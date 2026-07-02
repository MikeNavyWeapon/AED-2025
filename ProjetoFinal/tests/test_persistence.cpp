// Testes de persistência do Nível 2.
// Usa uma base de dados em memória (":memory:") para não tocar no disco.

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "ctikg/Database.hpp"
#include "ctikg/Graph.hpp"
#include "ctikg/SyntheticData.hpp"

namespace {

int g_failures = 0;
int g_checks = 0;

void check(bool condition, const std::string& message) {
    ++g_checks;
    if (!condition) {
        ++g_failures;
        std::cout << "  [FALHA] " << message << "\n";
    }
}

using namespace ctikg;

// Verifica que dois grafos têm exatamente os mesmos nós (id, tipo, nome, props)
// e as mesmas relações. É o coração do requisito "preservar a estrutura".
bool sameGraph(const Graph& a, const Graph& b) {
    if (a.nodeCount() != b.nodeCount()) return false;
    if (a.relationshipCount() != b.relationshipCount()) return false;

    for (const auto& [id, node] : a.nodes()) {
        const Node* other = b.getNode(id);
        if (!other) return false;
        if (other->type != node.type) return false;
        if (other->name != node.name) return false;
        if (other->properties != node.properties) return false;
    }

    auto key = [](const Relationship& r) {
        return r.id + "|" + r.source + "|" + r.target + "|" + r.type + "|" +
               std::to_string(r.confidence);
    };
    std::vector<std::string> ka, kb;
    for (const auto& r : a.relationships()) ka.push_back(key(r));
    for (const auto& r : b.relationships()) kb.push_back(key(r));
    std::sort(ka.begin(), ka.end());
    std::sort(kb.begin(), kb.end());
    return ka == kb;
}

void test_save_load_roundtrip() {
    std::cout << "test_save_load_roundtrip\n";
    Graph original;
    loadSyntheticData(original);

    Database db(":memory:");
    db.saveGraph(original);

    check(db.countNodes() == original.nodeCount(), "BD tem o mesmo nº de nós");
    check(db.countRelationships() == original.relationshipCount(), "BD tem o mesmo nº de relações");

    Graph reloaded = db.loadGraph();
    check(reloaded.nodeCount() == original.nodeCount(), "grafo recarregado: nº de nós igual");
    check(reloaded.relationshipCount() == original.relationshipCount(),
          "grafo recarregado: nº de relações igual");
    check(sameGraph(original, reloaded), "grafo recarregado é idêntico ao original");
}

void test_properties_preserved() {
    std::cout << "test_properties_preserved\n";
    Graph original;
    loadSyntheticData(original);
    Database db(":memory:");
    db.saveGraph(original);
    Graph reloaded = db.loadGraph();

    const Node* apt29 = reloaded.getNode("threat-actor--apt29");
    check(apt29 != nullptr, "APT29 existe após recarregar");
    check(apt29 && apt29->properties.count("aliases") > 0, "propriedade 'aliases' preservada");
    check(apt29 && apt29->properties.at("aliases") == "Cozy Bear, The Dukes",
          "valor de 'aliases' preservado");

    // A confiança das relações também tem de sobreviver à persistência.
    bool confidencePreserved = false;
    for (const auto& r : reloaded.relationships()) {
        if (r.id == "rel--007") confidencePreserved = (r.confidence == 99);
    }
    check(confidencePreserved, "confiança da relação preservada (rel--007 = 99)");
}

void test_find_nodes_by_type() {
    std::cout << "test_find_nodes_by_type\n";
    Graph original;
    loadSyntheticData(original);
    Database db(":memory:");
    db.saveGraph(original);

    auto actors = db.findNodesByType("threat-actor");
    check(actors.size() == original.nodesByType("threat-actor").size(),
          "pesquisa BD por tipo devolve o mesmo nº que em memória");
    check(db.findNodesByType("inexistente").empty(), "tipo inexistente devolve vazio");
}

void test_find_relationships() {
    std::cout << "test_find_relationships\n";
    Graph original;
    loadSyntheticData(original);
    Database db(":memory:");
    db.saveGraph(original);

    auto bySource = db.findRelationships("threat-actor--apt29", std::nullopt, std::nullopt);
    check(!bySource.empty(), "encontra relações por origem");

    auto byType = db.findRelationships(std::nullopt, std::nullopt, std::string("indicates"));
    check(!byType.empty(), "encontra relações por tipo");
    bool allIndicate =
        std::all_of(byType.begin(), byType.end(), [](const Relationship& r) { return r.type == "indicates"; });
    check(allIndicate, "todas as relações filtradas são do tipo pedido");

    auto byTarget = db.findRelationships(std::nullopt, std::string("malware--cozyduke"), std::nullopt);
    check(!byTarget.empty(), "encontra relações por destino");
}

void test_resave_replaces() {
    std::cout << "test_resave_replaces\n";
    Database db(":memory:");

    Graph g1;
    g1.addNode("threat-actor--a", "threat-actor", "A");
    db.saveGraph(g1);
    check(db.countNodes() == 1, "primeira gravação: 1 nó");

    Graph g2;
    loadSyntheticData(g2);
    db.saveGraph(g2);  // deve substituir, não acumular
    check(db.countNodes() == g2.nodeCount(), "segunda gravação substitui a primeira");
}

}  // namespace

int main() {
    std::cout << "=== Testes do Nível 2 (persistência) ===\n\n";

    test_save_load_roundtrip();
    test_properties_preserved();
    test_find_nodes_by_type();
    test_find_relationships();
    test_resave_replaces();

    std::cout << "\n--------------------------------------------\n";
    std::cout << "Verificações: " << g_checks << "  |  Falhas: " << g_failures << "\n";
    if (g_failures == 0) {
        std::cout << "RESULTADO: TODOS OS TESTES PASSARAM.\n";
        return 0;
    }
    std::cout << "RESULTADO: EXISTEM TESTES A FALHAR.\n";
    return 1;
}
