#pragma once

#include <optional>
#include <string>
#include <vector>

#include "ctikg/Graph.hpp"
#include "ctikg/Node.hpp"
#include "ctikg/Relationship.hpp"
#include "ctikg/Schema.hpp"

struct sqlite3;  // forward declaration (esconde o SQLite do cabeçalho)

namespace ctikg {

// Camada de persistência: grava e carrega o grafo numa base de dados SQLite.
//
// Está separada da lógica do grafo (Graph) — o Graph não sabe que existe uma
// base de dados, e a Database não conhece algoritmos. Esta separação prepara a
// arquitetura modular pedida no Nível 4.
//
// Em caso de erro de SQL, os métodos lançam std::runtime_error.
class Database {
public:
    // Abre (ou cria) a base de dados no caminho indicado e garante o esquema.
    // Use ":memory:" para uma base de dados temporária em memória.
    explicit Database(const std::string& path);
    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    // Cria as tabelas se ainda não existirem.
    void initSchema();

    // Grava todo o grafo (substitui o conteúdo atual da base de dados).
    // A operação é atómica (corre dentro de uma transação).
    void saveGraph(const Graph& graph);

    // Carrega o grafo completo a partir da base de dados.
    // O Schema é usado para validar/reconstruir; por omissão usa o esquema-padrão.
    Graph loadGraph(Schema schema = Schema()) const;

    // ----------------------------------------------- pesquisas ao nível da BD
    std::vector<Node> findNodesByType(const std::string& type) const;

    // Pesquisa relações por origem, destino e/ou tipo. Qualquer filtro pode ser
    // omitido (std::nullopt) para não restringir por esse campo.
    std::vector<Relationship> findRelationships(
        const std::optional<std::string>& source = std::nullopt,
        const std::optional<std::string>& target = std::nullopt,
        const std::optional<std::string>& type = std::nullopt) const;

    std::size_t countNodes() const;
    std::size_t countRelationships() const;

private:
    sqlite3* db_ = nullptr;

    void exec(const std::string& sql);  // executa SQL sem resultados
};

}  // namespace ctikg
