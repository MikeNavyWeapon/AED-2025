// Testes unitários da camada de serviço (Nível 4).

#include <iostream>
#include <string>

#include "ctikg/GraphService.hpp"

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

void test_create_and_validate() {
    std::cout << "test_create_and_validate\n";
    GraphService svc;

    check(svc.createNode("threat-actor--a", "threat-actor", "A").ok, "cria nó válido");
    check(!svc.createNode("", "threat-actor", "A").ok, "rejeita id vazio");
    check(!svc.createNode("n--1", "tipo-mau", "X").ok, "rejeita tipo inválido");
    check(!svc.createNode("threat-actor--a", "malware", "Dup").ok, "rejeita id duplicado");

    svc.createNode("malware--m", "malware", "M");
    check(svc.createRelationship("r1", "threat-actor--a", "malware--m", "uses").ok,
          "cria relação válida");
    check(!svc.createRelationship("r2", "malware--m", "threat-actor--a", "uses").ok,
          "rejeita relação fora da matriz");
    check(!svc.createRelationship("r3", "threat-actor--a", "malware--zzz", "uses").ok,
          "rejeita relação com destino inexistente");
}

void test_queries() {
    std::cout << "test_queries\n";
    GraphService svc;
    svc.loadSyntheticData();

    check(svc.getNode("threat-actor--apt29") != nullptr, "obtém nó por id");
    check(svc.getNode("nao-existe") == nullptr, "nó inexistente devolve nullptr");
    check(!svc.nodesByType("malware").empty(), "lista nós por tipo");
    check(!svc.neighbors("threat-actor--apt29").empty(), "lista vizinhos");

    auto p = svc.shortestPath("indicator--ip-185", "threat-actor--apt29");
    check(p.found, "calcula caminho entre dois nós");

    auto ranks = svc.ranking(3);
    check(ranks.size() == 3, "ranking respeita o limite top N");

    auto deg = svc.degreeRanking(3);
    check(deg.size() == 3, "centralidade por grau respeita o top N");

    auto rels = svc.relationships(std::string("threat-actor--apt29"), std::nullopt, std::nullopt);
    check(!rels.empty(), "lista relações por origem");
    auto byType = svc.relationships(std::nullopt, std::nullopt, std::string("indicates"));
    check(!byType.empty(), "lista relações por tipo");
}

void test_confidence_service() {
    std::cout << "test_confidence_service\n";
    GraphService svc;
    svc.createNode("threat-actor--a", "threat-actor", "A");
    svc.createNode("malware--m", "malware", "M");
    check(svc.createRelationship("r1", "threat-actor--a", "malware--m", "uses", 90).ok,
          "cria relação com confiança válida");
    check(!svc.createRelationship("r2", "threat-actor--a", "malware--m", "uses", 150).ok,
          "rejeita confiança fora de 0..100");
}

void test_update_node() {
    std::cout << "test_update_node\n";
    GraphService svc;
    svc.createNode("malware--m", "malware", "Nome Antigo");
    check(svc.updateNode("malware--m", "Nome Novo", {{"familia", "X"}}).ok, "atualiza nó existente");
    const Node* n = svc.getNode("malware--m");
    check(n && n->name == "Nome Novo", "nome atualizado");
    check(n && n->properties.at("familia") == "X", "propriedade atualizada");
    check(!svc.updateNode("nao-existe", "Y", {}).ok, "atualizar inexistente falha");
    check(!svc.updateNode("malware--m", "", {}).ok, "rejeita nome vazio");
}

void test_delete_and_reset() {
    std::cout << "test_delete_and_reset\n";
    GraphService svc;
    svc.loadSyntheticData();
    std::size_t n0 = svc.graph().nodeCount();

    // Apagar uma relação existente.
    check(svc.deleteRelationship("rel--001").ok, "apaga relação existente");
    check(!svc.deleteRelationship("rel--001").ok, "apagar de novo falha (já não existe)");

    // Apagar um nó remove-o e às suas relações incidentes.
    std::size_t before = svc.graph().relationshipCount();
    check(svc.deleteNode("malware--cozyduke").ok, "apaga nó existente");
    check(svc.getNode("malware--cozyduke") == nullptr, "nó deixou de existir");
    check(svc.graph().relationshipCount() < before, "relações incidentes foram removidas");
    check(!svc.deleteNode("nao-existe").ok, "apagar nó inexistente falha");

    // Repor volta ao dataset completo.
    svc.resetToSynthetic();
    check(svc.graph().nodeCount() == n0, "repor volta ao nº de nós inicial");
    check(svc.getNode("malware--cozyduke") != nullptr, "nó volta a existir após repor");
}

void test_stats_and_events() {
    std::cout << "test_stats_and_events\n";
    GraphService svc;
    svc.loadSyntheticData();

    auto s = svc.stats();
    check(s.nodes == svc.graph().nodeCount(), "stats conta nós");
    check(s.relationships == svc.graph().relationshipCount(), "stats conta relações");
    check(s.nodesByType["threat-actor"] >= 2, "stats agrupa por tipo");

    std::size_t before = svc.events().size();
    svc.createNode("malware--novo", "malware", "Novo");
    check(svc.events().size() == before + 1, "fila de eventos regista a criação do nó");
}

}  // namespace

int main() {
    std::cout << "=== Testes do Nível 4 (serviço) ===\n\n";

    test_create_and_validate();
    test_queries();
    test_confidence_service();
    test_update_node();
    test_delete_and_reset();
    test_stats_and_events();

    std::cout << "\n--------------------------------------------\n";
    std::cout << "Verificações: " << g_checks << "  |  Falhas: " << g_failures << "\n";
    if (g_failures == 0) {
        std::cout << "RESULTADO: TODOS OS TESTES PASSARAM.\n";
        return 0;
    }
    std::cout << "RESULTADO: EXISTEM TESTES A FALHAR.\n";
    return 1;
}
