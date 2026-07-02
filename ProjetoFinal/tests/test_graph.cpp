// Testes básicos do Nível 1.
// Usa um micro-harness próprio (sem dependências externas) para ser fácil de
// compilar com qualquer toolchain. Devolve código de saída != 0 se algo falhar.

#include <iostream>
#include <string>

#include "ctikg/Graph.hpp"
#include "ctikg/Schema.hpp"
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

// --------------------------------------------------------------------- testes

void test_add_node() {
    std::cout << "test_add_node\n";
    Graph g;
    std::string err;

    check(g.addNode("threat-actor--x", "threat-actor", "X", &err), "adiciona nó válido");
    check(g.hasNode("threat-actor--x"), "nó passa a existir");
    check(g.nodeCount() == 1, "contagem de nós = 1");

    // tipo inválido
    check(!g.addNode("n--1", "tipo-invalido", "Y", &err), "rejeita tipo desconhecido");
    // id duplicado
    check(!g.addNode("threat-actor--x", "malware", "Z", &err), "rejeita id duplicado");
    // id vazio
    check(!g.addNode("", "malware", "W", &err), "rejeita id vazio");
    check(g.nodeCount() == 1, "contagem mantém-se em 1 após rejeições");
}

void test_relationship_validation() {
    std::cout << "test_relationship_validation\n";
    Graph g;
    g.addNode("threat-actor--a", "threat-actor", "A");
    g.addNode("malware--m", "malware", "M");
    g.addNode("identity--i", "identity", "I");
    std::string err;

    // válida: threat-actor uses malware  (confiança 50 por omissão)
    check(g.addRelationship("r1", "threat-actor--a", "malware--m", "uses", 50, &err),
          "aceita threat-actor uses malware");
    check(g.relationshipCount() == 1, "contagem de relações = 1");

    // inválida pela matriz: malware uses threat-actor
    check(!g.addRelationship("r2", "malware--m", "threat-actor--a", "uses", 50, &err),
          "rejeita malware uses threat-actor");

    // tipo de relação desconhecido
    check(!g.addRelationship("r3", "threat-actor--a", "malware--m", "voa", 50, &err),
          "rejeita tipo de relação desconhecido");

    // nó inexistente
    check(!g.addRelationship("r4", "threat-actor--a", "malware--zzz", "uses", 50, &err),
          "rejeita destino inexistente");

    check(g.relationshipCount() == 1, "contagem de relações mantém-se em 1");
}

void test_neighbors() {
    std::cout << "test_neighbors\n";
    Graph g;
    g.addNode("threat-actor--a", "threat-actor", "A");
    g.addNode("malware--m1", "malware", "M1");
    g.addNode("malware--m2", "malware", "M2");
    g.addNode("indicator--i", "indicator", "I");
    g.addRelationship("r1", "threat-actor--a", "malware--m1", "uses");
    g.addRelationship("r2", "threat-actor--a", "malware--m2", "uses");
    g.addRelationship("r3", "indicator--i", "malware--m1", "indicates");

    check(g.outNeighbors("threat-actor--a").size() == 2, "ator tem 2 vizinhos de saída");
    check(g.inNeighbors("threat-actor--a").empty(), "ator não tem vizinhos de entrada");
    check(g.inNeighbors("malware--m1").size() == 2, "malware m1 tem 2 vizinhos de entrada");
    check(g.neighbors("malware--m1").size() == 2, "malware m1 tem 2 vizinhos no total");
}

void test_nodes_by_type() {
    std::cout << "test_nodes_by_type\n";
    Graph g;
    g.addNode("threat-actor--a", "threat-actor", "A");
    g.addNode("threat-actor--b", "threat-actor", "B");
    g.addNode("malware--m", "malware", "M");

    check(g.nodesByType("threat-actor").size() == 2, "2 atores");
    check(g.nodesByType("malware").size() == 1, "1 malware");
    check(g.nodesByType("campaign").empty(), "0 campanhas");
}

void test_schema() {
    std::cout << "test_schema\n";
    Schema s;
    check(s.isEntityType("threat-actor"), "threat-actor é tipo de entidade");
    check(!s.isEntityType("dragao"), "dragao não é tipo de entidade");
    check(s.isValidRelation("threat-actor", "uses", "malware"), "matriz: ator uses malware");
    check(s.isValidRelation("campaign", "targets", "identity"), "matriz: campaign targets identity");
    check(!s.isValidRelation("identity", "uses", "malware"), "matriz: identity uses malware inválida");
}

void test_synthetic_dataset() {
    std::cout << "test_synthetic_dataset\n";
    Graph g;
    loadSyntheticData(g);
    check(g.nodeCount() > 0, "dataset cria nós");
    check(g.relationshipCount() > 0, "dataset cria relações");
    check(g.hasNode("threat-actor--apt29"), "APT29 existe no dataset");
    check(!g.outNeighbors("threat-actor--apt29").empty(), "APT29 tem vizinhos");
}

}  // namespace

int main() {
    std::cout << "=== Testes do Nível 1 ===\n\n";

    test_schema();
    test_add_node();
    test_relationship_validation();
    test_neighbors();
    test_nodes_by_type();
    test_synthetic_dataset();

    std::cout << "\n--------------------------------------------\n";
    std::cout << "Verificações: " << g_checks << "  |  Falhas: " << g_failures << "\n";
    if (g_failures == 0) {
        std::cout << "RESULTADO: TODOS OS TESTES PASSARAM.\n";
        return 0;
    }
    std::cout << "RESULTADO: EXISTEM TESTES A FALHAR.\n";
    return 1;
}
