// Testes dos algoritmos de análise do Nível 3.

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

#include "ctikg/Analytics.hpp"
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

bool contains(const std::vector<std::string>& v, const std::string& x) {
    return std::find(v.begin(), v.end(), x) != v.end();
}

void test_bfs_dfs() {
    std::cout << "test_bfs_dfs\n";
    Graph g;
    loadSyntheticData(g);

    auto order = bfs(g, "threat-actor--apt29");
    check(!order.empty(), "BFS devolve nós");
    check(order.front() == "threat-actor--apt29", "BFS começa no nó inicial");
    check(contains(order, "malware--cozyduke"), "BFS alcança malware ligado");

    auto dorder = dfs(g, "threat-actor--apt29");
    check(dorder.size() == order.size(), "DFS visita o mesmo nº de nós que BFS (mesma componente)");
    check(dorder.front() == "threat-actor--apt29", "DFS começa no nó inicial");

    // Nó isolado: travessia só com ele próprio.
    Graph g2;
    g2.addNode("malware--solo", "malware", "Solo");
    check(bfs(g2, "malware--solo").size() == 1, "BFS de nó isolado tem tamanho 1");
}

void test_shortest_path() {
    std::cout << "test_shortest_path\n";
    Graph g;
    loadSyntheticData(g);

    // indicator--ip-185 --indicates--> cozyduke <--uses-- apt29  (2 arestas, não dirigido)
    Path p = shortestPath(g, "indicator--ip-185", "threat-actor--apt29");
    check(p.found, "encontra caminho indicador -> ator");
    check(p.nodes.size() == 3, "caminho tem 3 nós (2 arestas)");
    check(p.nodes.front() == "indicator--ip-185", "caminho começa no indicador");
    check(p.nodes.back() == "threat-actor--apt29", "caminho termina no ator");
    check(p.edgeTypes.size() == p.nodes.size() - 1, "nº de relações = nº de nós - 1");

    // Sem ligação possível.
    g.addNode("malware--ilha", "malware", "Ilha");
    Path none = shortestPath(g, "malware--ilha", "threat-actor--apt29");
    check(!none.found, "nó isolado não tem caminho para o ator");

    // Caminho dirigido pode não existir mesmo quando o não dirigido existe.
    Path dir = shortestPath(g, "indicator--ip-185", "threat-actor--apt29", /*directed=*/true);
    check(!dir.found, "no modo dirigido não há caminho indicador -> ator");
}

void test_strongest_path() {
    std::cout << "test_strongest_path\n";
    Graph g;
    // Dois caminhos M->D: um curto e fraco (M--exploits-->D, conf 10) e um mais
    // longo mas forte (M--uses-->C--exploits-->D, conf 90 cada). Todos válidos.
    g.addNode("malware--m", "malware", "M");
    g.addNode("attack-pattern--c", "attack-pattern", "C");
    g.addNode("vulnerability--d", "vulnerability", "D");
    g.addRelationship("r1", "malware--m", "vulnerability--d", "exploits", 10);  // atalho fraco
    g.addRelationship("r2", "malware--m", "attack-pattern--c", "uses", 90);
    g.addRelationship("r3", "attack-pattern--c", "vulnerability--d", "exploits", 90);

    Path best = strongestPath(g, "malware--m", "vulnerability--d");
    check(best.found, "encontra caminho de maior confiança");
    check(best.edgeConfidences.size() == best.nodes.size() - 1, "regista confiança por aresta");
    // O caminho mais fiável evita a aresta de confiança 10 (usa o percurso forte).
    int minConf = 100;
    for (int c : best.edgeConfidences) minConf = std::min(minConf, c);
    check(minConf >= 90, "escolhe o percurso de arestas fortes (min >= 90)");
    check(best.nodes.size() == 3, "o caminho mais fiável usa 2 arestas fortes");

    Path shortest = shortestPath(g, "malware--m", "vulnerability--d");
    check(shortest.nodes.size() == 2, "o caminho por nº de arestas usa o atalho direto");
}

void test_weighted_pagerank() {
    std::cout << "test_weighted_pagerank\n";
    Graph g;
    loadSyntheticData(g);
    auto w = weightedPageRank(g);
    check(w.size() == g.nodeCount(), "PageRank ponderado dá um score por nó");
    double sum = 0.0;
    for (const auto& [id, s] : w) sum += s;
    check(std::fabs(sum - 1.0) < 1e-6, "soma dos scores ponderados ~ 1");
    bool sorted = true;
    for (std::size_t i = 1; i < w.size(); ++i)
        if (w[i].second > w[i - 1].second + 1e-12) sorted = false;
    check(sorted, "ordenado por score decrescente");
}

void test_pagerank() {
    std::cout << "test_pagerank\n";
    Graph g;
    loadSyntheticData(g);

    auto ranks = pageRank(g);
    check(ranks.size() == g.nodeCount(), "PageRank devolve um score por nó");

    double sum = 0.0;
    for (const auto& [id, score] : ranks) sum += score;
    check(std::fabs(sum - 1.0) < 1e-6, "soma dos scores ~ 1");

    // Ordenado por score decrescente.
    bool sorted = true;
    for (std::size_t i = 1; i < ranks.size(); ++i)
        if (ranks[i].second > ranks[i - 1].second + 1e-12) sorted = false;
    check(sorted, "resultados ordenados por score decrescente");

    // CozyDuke recebe muitas ligações de entrada -> deve estar bem classificado.
    bool cozyTop5 = false;
    for (std::size_t i = 0; i < ranks.size() && i < 5; ++i)
        if (ranks[i].first == "malware--cozyduke") cozyTop5 = true;
    check(cozyTop5, "CozyDuke (muito referenciado) está no top 5");
}

void test_entity_resolution() {
    std::cout << "test_entity_resolution\n";
    Graph g;
    loadSyntheticData(g);

    auto pairs = resolveEntities(g);
    bool foundApt = false;
    for (const auto& [a, b] : pairs) {
        bool isApt = (a == "threat-actor--apt29" && b == "threat-actor--cozy-bear") ||
                     (a == "threat-actor--cozy-bear" && b == "threat-actor--apt29");
        if (isApt) foundApt = true;
    }
    check(foundApt, "deteta que APT29 e Cozy Bear são a mesma entidade");
}

void test_degree_centrality() {
    std::cout << "test_degree_centrality\n";
    Graph g;
    g.addNode("threat-actor--a", "threat-actor", "A");
    g.addNode("malware--m1", "malware", "M1");
    g.addNode("malware--m2", "malware", "M2");
    g.addNode("indicator--i", "indicator", "I");
    g.addRelationship("r1", "threat-actor--a", "malware--m1", "uses");
    g.addRelationship("r2", "threat-actor--a", "malware--m2", "uses");
    g.addRelationship("r3", "indicator--i", "malware--m1", "indicates");

    auto deg = degreeCentrality(g);
    check(deg.size() == g.nodeCount(), "centralidade calculada para todos os nós");
    // O ator tem 2 de saída; m1 tem 2 de entrada -> empatados no topo (total 2).
    check(deg.front().total == 2, "o nó com mais ligações tem total 2");
    // Ordenação por total decrescente.
    bool sorted = true;
    for (std::size_t i = 1; i < deg.size(); ++i)
        if (deg[i].total > deg[i - 1].total) sorted = false;
    check(sorted, "resultados ordenados por grau total decrescente");

    // Verifica os graus do ator concretamente.
    for (const auto& d : deg)
        if (d.id == "threat-actor--a") {
            check(d.out == 2 && d.in == 0 && d.total == 2, "graus do ator: out=2, in=0");
        }
}

void test_confidence() {
    std::cout << "test_confidence\n";
    Graph g;
    loadSyntheticData(g);
    // A relação rel--001 (APT29 uses CozyDuke) foi criada com confiança 95.
    bool found = false;
    for (const auto& r : g.relationships())
        if (r.id == "rel--001") { found = true; check(r.confidence == 95, "confiança guardada na relação"); }
    check(found, "relação com confiança existe");
    // A confiança propaga-se às arestas das listas de adjacência.
    for (const Edge& e : g.outNeighbors("threat-actor--apt29"))
        if (e.neighborId == "malware--cozyduke")
            check(e.confidence == 95, "confiança presente na aresta de adjacência");
}

void test_connected_components() {
    std::cout << "test_connected_components\n";
    Graph g;
    g.addNode("threat-actor--a", "threat-actor", "A");
    g.addNode("malware--m", "malware", "M");
    g.addRelationship("r1", "threat-actor--a", "malware--m", "uses");
    g.addNode("malware--ilha", "malware", "Ilha");  // nó isolado

    auto comps = connectedComponents(g);
    check(comps.size() == 2, "deteta 2 comunidades (uma ligada + uma isolada)");
    check(comps.front().size() == 2, "a maior comunidade tem 2 nós");

    // O dataset sintético deve formar uma única grande comunidade.
    Graph full;
    loadSyntheticData(full);
    auto fullComps = connectedComponents(full);
    check(!fullComps.empty(), "dataset tem pelo menos uma comunidade");
    check(fullComps.front().size() >= full.nodeCount() / 2,
          "a maior comunidade cobre grande parte do grafo");
}

void test_edge_cases() {
    std::cout << "test_edge_cases\n";
    // Grafo vazio: PageRank não rebenta e devolve vazio.
    Graph empty;
    check(pageRank(empty).empty(), "PageRank de grafo vazio é vazio");
    check(connectedComponents(empty).empty(), "grafo vazio não tem comunidades");

    // Caminho de um nó para si próprio.
    Graph g;
    g.addNode("malware--m", "malware", "M");
    Path self = shortestPath(g, "malware--m", "malware--m");
    check(self.found && self.nodes.size() == 1, "caminho de um nó para si próprio tem 1 nó");

    // Nó inexistente em BFS.
    check(bfs(g, "nao-existe").empty(), "BFS de nó inexistente é vazio");
}

void test_analytical_queries() {
    std::cout << "test_analytical_queries\n";
    Graph g;
    loadSyntheticData(g);

    auto actors = actorsUsingMalware(g, "malware--cozyduke");
    check(contains(actors, "threat-actor--apt29"), "APT29 usa CozyDuke");

    auto inds = indicatorsPointingTo(g, "malware--cozyduke");
    check(inds.size() >= 2, "CozyDuke tem pelo menos 2 indicadores a apontar-lhe");
    check(contains(inds, "indicator--ip-185"), "o IP 185.x é um dos indicadores");
}

}  // namespace

int main() {
    std::cout << "=== Testes do Nível 3 (algoritmos) ===\n\n";

    test_bfs_dfs();
    test_shortest_path();
    test_pagerank();
    test_strongest_path();
    test_weighted_pagerank();
    test_degree_centrality();
    test_confidence();
    test_connected_components();
    test_edge_cases();
    test_entity_resolution();
    test_analytical_queries();

    std::cout << "\n--------------------------------------------\n";
    std::cout << "Verificações: " << g_checks << "  |  Falhas: " << g_failures << "\n";
    if (g_failures == 0) {
        std::cout << "RESULTADO: TODOS OS TESTES PASSARAM.\n";
        return 0;
    }
    std::cout << "RESULTADO: EXISTEM TESTES A FALHAR.\n";
    return 1;
}
