#pragma once

#include <cstddef>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "ctikg/Analytics.hpp"
#include "ctikg/Graph.hpp"
#include "ctikg/Relationship.hpp"

namespace ctikg {

// Resultado de uma operação que pode falhar de forma controlada.
struct ServiceResult {
    bool ok = false;
    std::string error;

    static ServiceResult success() { return {true, ""}; }
    static ServiceResult failure(std::string e) { return {false, std::move(e)}; }
};

// Camada de serviço: ponto de acesso único e modular ao sistema.
//
// Orquestra a lógica do grafo (Graph), os algoritmos (Analytics) e, quando
// pedido, a persistência (Database). Valida entradas e traduz erros em
// ServiceResult, escondendo os detalhes das camadas inferiores.
//
// Pode ser usada diretamente em C++ ("acesso local") ou por trás de uma API
// REST (ver src/api_main.cpp).
class GraphService {
public:
    GraphService() = default;

    // --------------------------------------------------------------- dados
    void loadSyntheticData();
    void resetToSynthetic();                          // limpa e recarrega o dataset
    void loadFromDatabase(const std::string& path);  // substitui o grafo atual
    void saveToDatabase(const std::string& path) const;

    // ------------------------------------------------------------ mutações
    ServiceResult createNode(const std::string& id, const std::string& type,
                             const std::string& name,
                             const std::map<std::string, std::string>& properties = {});

    ServiceResult createRelationship(const std::string& id, const std::string& source,
                                     const std::string& target, const std::string& type,
                                     int confidence = 50);

    ServiceResult updateNode(const std::string& id, const std::string& name,
                             const std::map<std::string, std::string>& properties = {});

    ServiceResult deleteNode(const std::string& id);
    ServiceResult deleteRelationship(const std::string& id);

    // ------------------------------------------------------------ consultas
    const Node* getNode(const std::string& id) const;
    std::vector<const Node*> nodesByType(const std::string& type) const;
    std::vector<Edge> neighbors(const std::string& id) const;
    Path shortestPath(const std::string& from, const std::string& to) const;
    Path strongestPath(const std::string& from, const std::string& to) const;
    std::vector<std::pair<std::string, double>> ranking(std::size_t topN = 10) const;
    std::vector<std::pair<std::string, double>> weightedRanking(std::size_t topN = 10) const;
    std::vector<DegreeCentrality> degreeRanking(std::size_t topN = 10) const;

    // Lista relações, com filtros opcionais por origem, destino e/ou tipo.
    std::vector<Relationship> relationships(
        const std::optional<std::string>& source = std::nullopt,
        const std::optional<std::string>& target = std::nullopt,
        const std::optional<std::string>& type = std::nullopt) const;

    // ------------------------------------------------------------- estatísticas
    struct Stats {
        std::size_t nodes = 0;
        std::size_t relationships = 0;
        std::size_t components = 0;  // nº de comunidades (componentes conexas)
        std::map<std::string, std::size_t> nodesByType;
    };
    Stats stats() const;

    // -------------------------------------------------- fila de eventos (bónus)
    const std::vector<std::string>& events() const { return events_; }

    const Graph& graph() const { return graph_; }

private:
    Graph graph_;
    std::vector<std::string> events_;  // registo simples de alterações ao grafo

    void logEvent(std::string message) { events_.push_back(std::move(message)); }
};

}  // namespace ctikg
