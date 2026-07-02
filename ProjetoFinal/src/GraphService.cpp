#include "ctikg/GraphService.hpp"

#include "ctikg/Database.hpp"
#include "ctikg/SyntheticData.hpp"

namespace ctikg {

// ------------------------------------------------------------------- dados

void GraphService::loadSyntheticData() {
    ctikg::loadSyntheticData(graph_);
    logEvent("Dados sintéticos carregados (" + std::to_string(graph_.nodeCount()) + " nós).");
}

void GraphService::resetToSynthetic() {
    graph_ = Graph();
    events_.clear();
    ctikg::loadSyntheticData(graph_);
    logEvent("Grafo reposto para os dados sintéticos (" + std::to_string(graph_.nodeCount()) +
             " nós).");
}

void GraphService::loadFromDatabase(const std::string& path) {
    Database db(path);
    graph_ = db.loadGraph();
    logEvent("Grafo carregado da base de dados '" + path + "'.");
}

void GraphService::saveToDatabase(const std::string& path) const {
    Database db(path);
    db.saveGraph(graph_);
}

// ---------------------------------------------------------------- mutações

ServiceResult GraphService::createNode(const std::string& id, const std::string& type,
                                       const std::string& name,
                                       const std::map<std::string, std::string>& properties) {
    if (id.empty()) return ServiceResult::failure("O campo 'id' é obrigatório.");
    if (type.empty()) return ServiceResult::failure("O campo 'type' é obrigatório.");
    if (name.empty()) return ServiceResult::failure("O campo 'name' é obrigatório.");

    Node node(id, type, name);
    node.properties = properties;

    std::string error;
    if (!graph_.addNode(node, &error)) return ServiceResult::failure(error);

    logEvent("Nó criado: " + id + " (" + type + ").");
    return ServiceResult::success();
}

ServiceResult GraphService::createRelationship(const std::string& id, const std::string& source,
                                               const std::string& target, const std::string& type,
                                               int confidence) {
    if (id.empty()) return ServiceResult::failure("O campo 'id' é obrigatório.");
    if (source.empty() || target.empty())
        return ServiceResult::failure("Os campos 'source' e 'target' são obrigatórios.");
    if (type.empty()) return ServiceResult::failure("O campo 'type' é obrigatório.");
    if (confidence < 0 || confidence > 100)
        return ServiceResult::failure("O campo 'confidence' deve estar entre 0 e 100.");

    std::string error;
    if (!graph_.addRelationship(id, source, target, type, confidence, &error))
        return ServiceResult::failure(error);

    logEvent("Relação criada: " + source + " --" + type + "--> " + target + ".");
    return ServiceResult::success();
}

ServiceResult GraphService::updateNode(const std::string& id, const std::string& name,
                                       const std::map<std::string, std::string>& properties) {
    if (name.empty()) return ServiceResult::failure("O campo 'name' é obrigatório.");
    if (!graph_.updateNode(id, name, properties))
        return ServiceResult::failure("Nó inexistente: " + id);
    logEvent("Nó atualizado: " + id + ".");
    return ServiceResult::success();
}

ServiceResult GraphService::deleteNode(const std::string& id) {
    if (!graph_.removeNode(id)) return ServiceResult::failure("Nó inexistente: " + id);
    logEvent("Nó removido: " + id + ".");
    return ServiceResult::success();
}

ServiceResult GraphService::deleteRelationship(const std::string& id) {
    if (!graph_.removeRelationship(id)) return ServiceResult::failure("Relação inexistente: " + id);
    logEvent("Relação removida: " + id + ".");
    return ServiceResult::success();
}

// ---------------------------------------------------------------- consultas

const Node* GraphService::getNode(const std::string& id) const { return graph_.getNode(id); }

std::vector<const Node*> GraphService::nodesByType(const std::string& type) const {
    return graph_.nodesByType(type);
}

std::vector<Edge> GraphService::neighbors(const std::string& id) const {
    return graph_.neighbors(id);
}

Path GraphService::shortestPath(const std::string& from, const std::string& to) const {
    return ctikg::shortestPath(graph_, from, to);
}

Path GraphService::strongestPath(const std::string& from, const std::string& to) const {
    return ctikg::strongestPath(graph_, from, to);
}

std::vector<std::pair<std::string, double>> GraphService::ranking(std::size_t topN) const {
    auto ranks = pageRank(graph_);
    if (ranks.size() > topN) ranks.resize(topN);
    return ranks;
}

std::vector<std::pair<std::string, double>> GraphService::weightedRanking(std::size_t topN) const {
    auto ranks = weightedPageRank(graph_);
    if (ranks.size() > topN) ranks.resize(topN);
    return ranks;
}

std::vector<DegreeCentrality> GraphService::degreeRanking(std::size_t topN) const {
    auto degrees = degreeCentrality(graph_);
    if (degrees.size() > topN) degrees.resize(topN);
    return degrees;
}

std::vector<Relationship> GraphService::relationships(const std::optional<std::string>& source,
                                                     const std::optional<std::string>& target,
                                                     const std::optional<std::string>& type) const {
    std::vector<Relationship> result;
    for (const auto& r : graph_.relationships()) {
        if (source && r.source != *source) continue;
        if (target && r.target != *target) continue;
        if (type && r.type != *type) continue;
        result.push_back(r);
    }
    return result;
}

// --------------------------------------------------------------- estatísticas

GraphService::Stats GraphService::stats() const {
    Stats s;
    s.nodes = graph_.nodeCount();
    s.relationships = graph_.relationshipCount();
    s.components = connectedComponents(graph_).size();
    for (const auto& [id, node] : graph_.nodes()) {
        s.nodesByType[node.type]++;
    }
    return s;
}

}  // namespace ctikg
