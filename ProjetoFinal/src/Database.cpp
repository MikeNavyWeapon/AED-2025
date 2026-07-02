#include "ctikg/Database.hpp"

#include <sqlite3.h>

#include <stdexcept>
#include <string>

namespace ctikg {

namespace {

// Lança uma exceção com a mensagem de erro do SQLite, se rc indicar falha.
void check(sqlite3* db, int rc, const std::string& context) {
    if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
        std::string msg = context + ": " + (db ? sqlite3_errmsg(db) : "erro desconhecido");
        throw std::runtime_error(msg);
    }
}

// RAII para um statement preparado: garante sqlite3_finalize.
class Stmt {
public:
    Stmt(sqlite3* db, const std::string& sql) : db_(db) {
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt_, nullptr);
        check(db, rc, "prepare");
    }
    ~Stmt() { sqlite3_finalize(stmt_); }
    Stmt(const Stmt&) = delete;
    Stmt& operator=(const Stmt&) = delete;

    void bind(int i, const std::string& value) {
        // SQLITE_TRANSIENT: o SQLite faz a sua própria cópia do texto.
        check(db_, sqlite3_bind_text(stmt_, i, value.c_str(), -1, SQLITE_TRANSIENT), "bind");
    }

    void bindInt(int i, int value) { check(db_, sqlite3_bind_int(stmt_, i, value), "bindInt"); }

    bool step() {
        int rc = sqlite3_step(stmt_);
        if (rc == SQLITE_ROW) return true;
        if (rc == SQLITE_DONE) return false;
        check(db_, rc, "step");
        return false;
    }

    std::string columnText(int i) const {
        const unsigned char* text = sqlite3_column_text(stmt_, i);
        return text ? reinterpret_cast<const char*>(text) : "";
    }

    int columnInt(int i) const { return sqlite3_column_int(stmt_, i); }

    sqlite3_stmt* raw() { return stmt_; }

private:
    sqlite3* db_;
    sqlite3_stmt* stmt_ = nullptr;
};

}  // namespace

// --------------------------------------------------------------- ciclo de vida

Database::Database(const std::string& path) {
    int rc = sqlite3_open(path.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::string msg = db_ ? sqlite3_errmsg(db_) : "não foi possível abrir a base de dados";
        sqlite3_close(db_);
        db_ = nullptr;
        throw std::runtime_error("sqlite3_open: " + msg);
    }
    exec("PRAGMA foreign_keys = ON;");
    initSchema();
}

Database::~Database() {
    if (db_) sqlite3_close(db_);
}

void Database::exec(const std::string& sql) {
    char* errmsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errmsg);
    if (rc != SQLITE_OK) {
        std::string msg = errmsg ? errmsg : "erro de SQL";
        sqlite3_free(errmsg);
        throw std::runtime_error("exec: " + msg);
    }
}

void Database::initSchema() {
    exec(
        "CREATE TABLE IF NOT EXISTS nodes ("
        "  id TEXT PRIMARY KEY,"
        "  type TEXT NOT NULL,"
        "  name TEXT NOT NULL);");
    exec(
        "CREATE TABLE IF NOT EXISTS node_properties ("
        "  node_id TEXT NOT NULL,"
        "  key TEXT NOT NULL,"
        "  value TEXT,"
        "  PRIMARY KEY (node_id, key),"
        "  FOREIGN KEY (node_id) REFERENCES nodes(id) ON DELETE CASCADE);");
    exec(
        "CREATE TABLE IF NOT EXISTS relationships ("
        "  id TEXT PRIMARY KEY,"
        "  source TEXT NOT NULL,"
        "  target TEXT NOT NULL,"
        "  type TEXT NOT NULL,"
        "  confidence INTEGER NOT NULL DEFAULT 50,"
        "  FOREIGN KEY (source) REFERENCES nodes(id) ON DELETE CASCADE,"
        "  FOREIGN KEY (target) REFERENCES nodes(id) ON DELETE CASCADE);");
    exec("CREATE INDEX IF NOT EXISTS idx_nodes_type ON nodes(type);");
    exec("CREATE INDEX IF NOT EXISTS idx_rel_source ON relationships(source);");
    exec("CREATE INDEX IF NOT EXISTS idx_rel_target ON relationships(target);");
    exec("CREATE INDEX IF NOT EXISTS idx_rel_type ON relationships(type);");
}

// --------------------------------------------------------------------- gravar

void Database::saveGraph(const Graph& graph) {
    exec("BEGIN TRANSACTION;");
    try {
        // Substitui completamente o conteúdo anterior.
        exec("DELETE FROM relationships;");
        exec("DELETE FROM node_properties;");
        exec("DELETE FROM nodes;");

        {
            Stmt insertNode(db_, "INSERT INTO nodes (id, type, name) VALUES (?, ?, ?);");
            Stmt insertProp(db_,
                            "INSERT INTO node_properties (node_id, key, value) VALUES (?, ?, ?);");
            for (const auto& [id, node] : graph.nodes()) {
                sqlite3_reset(insertNode.raw());
                insertNode.bind(1, node.id);
                insertNode.bind(2, node.type);
                insertNode.bind(3, node.name);
                insertNode.step();

                for (const auto& [key, value] : node.properties) {
                    sqlite3_reset(insertProp.raw());
                    insertProp.bind(1, node.id);
                    insertProp.bind(2, key);
                    insertProp.bind(3, value);
                    insertProp.step();
                }
            }
        }
        {
            Stmt insertRel(db_,
                           "INSERT INTO relationships (id, source, target, type, confidence) "
                           "VALUES (?, ?, ?, ?, ?);");
            for (const auto& rel : graph.relationships()) {
                sqlite3_reset(insertRel.raw());
                insertRel.bind(1, rel.id);
                insertRel.bind(2, rel.source);
                insertRel.bind(3, rel.target);
                insertRel.bind(4, rel.type);
                insertRel.bindInt(5, rel.confidence);
                insertRel.step();
            }
        }
        exec("COMMIT;");
    } catch (...) {
        exec("ROLLBACK;");
        throw;
    }
}

// ------------------------------------------------------------------- carregar

Graph Database::loadGraph(Schema schema) const {
    Graph graph(std::move(schema));

    // 1) Nós (têm de existir antes das relações).
    {
        Stmt q(db_, "SELECT id, type, name FROM nodes;");
        while (q.step()) {
            Node node(q.columnText(0), q.columnText(1), q.columnText(2));
            graph.addNode(node);
        }
    }
    // 2) Propriedades dos nós.
    {
        Stmt q(db_, "SELECT node_id, key, value FROM node_properties;");
        while (q.step()) {
            // const_cast: estamos a reconstruir o estado, não a mutar logicamente.
            const Node* n = graph.getNode(q.columnText(0));
            if (n) const_cast<Node*>(n)->properties[q.columnText(1)] = q.columnText(2);
        }
    }
    // 3) Relações.
    {
        Stmt q(db_, "SELECT id, source, target, type, confidence FROM relationships;");
        while (q.step()) {
            graph.addRelationship(q.columnText(0), q.columnText(1), q.columnText(2),
                                  q.columnText(3), q.columnInt(4));
        }
    }
    return graph;
}

// ------------------------------------------------------------------ pesquisas

std::vector<Node> Database::findNodesByType(const std::string& type) const {
    std::vector<Node> result;
    Stmt q(db_, "SELECT id, type, name FROM nodes WHERE type = ?;");
    q.bind(1, type);
    while (q.step()) {
        result.emplace_back(q.columnText(0), q.columnText(1), q.columnText(2));
    }
    return result;
}

std::vector<Relationship> Database::findRelationships(const std::optional<std::string>& source,
                                                      const std::optional<std::string>& target,
                                                      const std::optional<std::string>& type) const {
    std::string sql = "SELECT id, source, target, type, confidence FROM relationships WHERE 1=1";
    if (source) sql += " AND source = ?";
    if (target) sql += " AND target = ?";
    if (type) sql += " AND type = ?";
    sql += ";";

    Stmt q(db_, sql);
    int idx = 1;
    if (source) q.bind(idx++, *source);
    if (target) q.bind(idx++, *target);
    if (type) q.bind(idx++, *type);

    std::vector<Relationship> result;
    while (q.step()) {
        result.emplace_back(q.columnText(0), q.columnText(1), q.columnText(2), q.columnText(3),
                            q.columnInt(4));
    }
    return result;
}

std::size_t Database::countNodes() const {
    Stmt q(db_, "SELECT COUNT(*) FROM nodes;");
    q.step();
    return static_cast<std::size_t>(sqlite3_column_int64(q.raw(), 0));
}

std::size_t Database::countRelationships() const {
    Stmt q(db_, "SELECT COUNT(*) FROM relationships;");
    q.step();
    return static_cast<std::size_t>(sqlite3_column_int64(q.raw(), 0));
}

}  // namespace ctikg
