#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "ctikg/Node.hpp"
#include "ctikg/Relationship.hpp"
#include "ctikg/Schema.hpp"

namespace ctikg {

// Uma aresta na lista de adjacência: descreve a ligação a um nó vizinho.
struct Edge {
    std::string relationshipId;  // id da relação que originou esta aresta
    std::string neighborId;      // id do nó vizinho (o "outro lado" da relação)
    std::string type;            // tipo da relação (ex.: "uses")
    bool outgoing;               // true: aresta de saída; false: de entrada
    int confidence = 50;         // grau de confiança da relação (0..100)
};

// Grafo de conhecimento dirigido, em memória.
//
// Estruturas de dados internas:
//   - nodes_:         mapa id -> Node (acesso O(1) a cada entidade).
//   - relationships_: lista de todas as relações (origem da verdade).
//   - out_ / in_:     LISTAS DE ADJACÊNCIA (saída e entrada), para navegação
//                     eficiente entre vizinhos, como pedido no enunciado.
class Graph {
public:
    explicit Graph(Schema schema = Schema());

    // ---------------------------------------------------------------- construção
    // Adiciona um nó. Falha (devolve false) se o id for vazio, já existir, ou o
    // tipo não pertencer ao Schema. Em caso de erro, *error recebe a explicação.
    bool addNode(const Node& node, std::string* error = nullptr);
    bool addNode(const std::string& id, const std::string& type, const std::string& name,
                 std::string* error = nullptr);

    // Adiciona uma relação dirigida. Falha se origem/destino não existirem, se o
    // tipo de relação for desconhecido, ou se o triplo não for permitido pelo
    // Schema (validação da matriz de relações).
    bool addRelationship(const Relationship& rel, std::string* error = nullptr);
    bool addRelationship(const std::string& id, const std::string& source,
                         const std::string& target, const std::string& type,
                         int confidence = 50, std::string* error = nullptr);

    // Atualiza o nome e as propriedades de um nó existente. false se não existir.
    bool updateNode(const std::string& id, const std::string& name,
                    const std::map<std::string, std::string>& properties);

    // Remove um nó e todas as relações incidentes. Devolve false se não existir.
    bool removeNode(const std::string& id);
    // Remove uma relação pelo seu id. Devolve false se não existir.
    bool removeRelationship(const std::string& id);

    // ----------------------------------------------------------------- consultas
    bool hasNode(const std::string& id) const;
    const Node* getNode(const std::string& id) const;  // nullptr se não existir

    // Todos os nós de um dado tipo (ex.: todos os "threat-actor").
    std::vector<const Node*> nodesByType(const std::string& type) const;

    // Vizinhos por arestas de saída, de entrada, ou ambos.
    std::vector<Edge> outNeighbors(const std::string& id) const;
    std::vector<Edge> inNeighbors(const std::string& id) const;
    std::vector<Edge> neighbors(const std::string& id) const;

    // -------------------------------------------------------------------- acesso
    std::size_t nodeCount() const { return nodes_.size(); }
    std::size_t relationshipCount() const { return relationships_.size(); }

    const std::unordered_map<std::string, Node>& nodes() const { return nodes_; }
    const std::vector<Relationship>& relationships() const { return relationships_; }
    const Schema& schema() const { return schema_; }

private:
    Schema schema_;
    std::unordered_map<std::string, Node> nodes_;
    std::vector<Relationship> relationships_;
    std::unordered_map<std::string, std::vector<Edge>> out_;
    std::unordered_map<std::string, std::vector<Edge>> in_;

    // Reconstrói as listas de adjacência a partir de relationships_.
    void rebuildAdjacency();
};

}  // namespace ctikg
