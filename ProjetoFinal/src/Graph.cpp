#include "ctikg/Graph.hpp"

#include <algorithm>

namespace ctikg {

namespace {
// Pequeno utilitário: escreve uma mensagem de erro se o ponteiro for válido.
void setError(std::string* error, std::string message) {
    if (error) *error = std::move(message);
}
}  // namespace

Graph::Graph(Schema schema) : schema_(std::move(schema)) {}

// ---------------------------------------------------------------------- nós

bool Graph::addNode(const Node& node, std::string* error) {
    if (node.id.empty()) {
        setError(error, "O nó precisa de um id não vazio.");
        return false;
    }
    if (nodes_.count(node.id) > 0) {
        setError(error, "Já existe um nó com o id '" + node.id + "'.");
        return false;
    }
    if (!schema_.isEntityType(node.type)) {
        setError(error, "Tipo de entidade desconhecido: '" + node.type + "'.");
        return false;
    }
    nodes_.emplace(node.id, node);
    return true;
}

bool Graph::addNode(const std::string& id, const std::string& type, const std::string& name,
                    std::string* error) {
    return addNode(Node(id, type, name), error);
}

// ------------------------------------------------------------------ relações

bool Graph::addRelationship(const Relationship& rel, std::string* error) {
    if (rel.id.empty()) {
        setError(error, "A relação precisa de um id não vazio.");
        return false;
    }
    const Node* src = getNode(rel.source);
    if (!src) {
        setError(error, "Nó de origem inexistente: '" + rel.source + "'.");
        return false;
    }
    const Node* dst = getNode(rel.target);
    if (!dst) {
        setError(error, "Nó de destino inexistente: '" + rel.target + "'.");
        return false;
    }
    if (!schema_.isRelationType(rel.type)) {
        setError(error, "Tipo de relação desconhecido: '" + rel.type + "'.");
        return false;
    }
    // Validação da matriz de relações: (tipoOrigem, relação, tipoDestino).
    if (!schema_.isValidRelation(src->type, rel.type, dst->type)) {
        setError(error, "Relação inválida: " + src->type + " --" + rel.type + "--> " +
                            dst->type + " não é permitida pelo esquema.");
        return false;
    }

    relationships_.push_back(rel);
    // Atualiza as listas de adjacência (saída no source, entrada no target).
    out_[rel.source].push_back(Edge{rel.id, rel.target, rel.type, /*outgoing=*/true, rel.confidence});
    in_[rel.target].push_back(Edge{rel.id, rel.source, rel.type, /*outgoing=*/false, rel.confidence});
    return true;
}

bool Graph::addRelationship(const std::string& id, const std::string& source,
                            const std::string& target, const std::string& type, int confidence,
                            std::string* error) {
    return addRelationship(Relationship(id, source, target, type, confidence), error);
}

// ---------------------------------------------------------------- atualização

bool Graph::updateNode(const std::string& id, const std::string& name,
                       const std::map<std::string, std::string>& properties) {
    auto it = nodes_.find(id);
    if (it == nodes_.end()) return false;
    it->second.name = name;
    it->second.properties = properties;
    return true;  // id e tipo não mudam; as listas de adjacência guardam ids, não nomes
}

// -------------------------------------------------------------------- remoção

void Graph::rebuildAdjacency() {
    out_.clear();
    in_.clear();
    for (const auto& rel : relationships_) {
        out_[rel.source].push_back(Edge{rel.id, rel.target, rel.type, true, rel.confidence});
        in_[rel.target].push_back(Edge{rel.id, rel.source, rel.type, false, rel.confidence});
    }
}

bool Graph::removeRelationship(const std::string& id) {
    auto it = std::find_if(relationships_.begin(), relationships_.end(),
                           [&](const Relationship& r) { return r.id == id; });
    if (it == relationships_.end()) return false;
    relationships_.erase(it);
    rebuildAdjacency();
    return true;
}

bool Graph::removeNode(const std::string& id) {
    if (nodes_.count(id) == 0) return false;
    nodes_.erase(id);
    // Remove todas as relações que tocam neste nó.
    relationships_.erase(
        std::remove_if(relationships_.begin(), relationships_.end(),
                       [&](const Relationship& r) { return r.source == id || r.target == id; }),
        relationships_.end());
    rebuildAdjacency();
    return true;
}

// ------------------------------------------------------------------ consultas

bool Graph::hasNode(const std::string& id) const { return nodes_.count(id) > 0; }

const Node* Graph::getNode(const std::string& id) const {
    auto it = nodes_.find(id);
    return it == nodes_.end() ? nullptr : &it->second;
}

std::vector<const Node*> Graph::nodesByType(const std::string& type) const {
    std::vector<const Node*> result;
    for (const auto& [id, node] : nodes_) {
        if (node.type == type) result.push_back(&node);
    }
    return result;
}

std::vector<Edge> Graph::outNeighbors(const std::string& id) const {
    auto it = out_.find(id);
    return it == out_.end() ? std::vector<Edge>{} : it->second;
}

std::vector<Edge> Graph::inNeighbors(const std::string& id) const {
    auto it = in_.find(id);
    return it == in_.end() ? std::vector<Edge>{} : it->second;
}

std::vector<Edge> Graph::neighbors(const std::string& id) const {
    std::vector<Edge> result = outNeighbors(id);
    std::vector<Edge> incoming = inNeighbors(id);
    result.insert(result.end(), incoming.begin(), incoming.end());
    return result;
}

}  // namespace ctikg
