// Aplicação de demonstração por consola do grafo de conhecimento de CTI.
// Nível 1: grafo em memória, validação de relações e consultas básicas.

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "ctikg/Analytics.hpp"
#include "ctikg/Database.hpp"
#include "ctikg/Graph.hpp"
#include "ctikg/SyntheticData.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

using namespace ctikg;

void enableUtf8Console() {
#ifdef _WIN32
    // Garante que os acentos do português aparecem corretamente na consola.
    SetConsoleOutputCP(CP_UTF8);
#endif
}

void printHeader(const std::string& title) {
    std::cout << "\n========================================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "========================================================\n";
}

// Mostra o nome de um nó a partir do seu id (ou o próprio id, se não existir).
std::string nameOf(const Graph& g, const std::string& id) {
    const Node* n = g.getNode(id);
    return n ? n->name : id;
}

void showStatistics(const Graph& g) {
    printHeader("Estatísticas gerais do grafo");
    std::cout << "Total de nós:      " << g.nodeCount() << "\n";
    std::cout << "Total de relações: " << g.relationshipCount() << "\n";
    std::cout << "Comunidades:       " << connectedComponents(g).size()
              << " (componentes conexas)\n\n";

    std::cout << "Nós por tipo:\n";
    for (const auto& type : g.schema().entityTypes()) {
        auto nodes = g.nodesByType(type);
        if (!nodes.empty()) {
            std::cout << "  - " << type << ": " << nodes.size() << "\n";
        }
    }
}

void listNodesByType(const Graph& g, const std::string& type) {
    printHeader("Nós do tipo '" + type + "'");
    auto nodes = g.nodesByType(type);
    std::sort(nodes.begin(), nodes.end(),
              [](const Node* a, const Node* b) { return a->name < b->name; });
    for (const Node* n : nodes) {
        std::cout << "  [" << n->id << "]  " << n->name;
        auto it = n->properties.find("aliases");
        if (it != n->properties.end()) std::cout << "   (aliases: " << it->second << ")";
        std::cout << "\n";
    }
}

void showNeighbors(const Graph& g, const std::string& id) {
    printHeader("Vizinhos de " + nameOf(g, id));
    const Node* node = g.getNode(id);
    if (!node) {
        std::cout << "Nó inexistente: " << id << "\n";
        return;
    }
    std::cout << "Entidade: " << node->name << " (" << node->type << ")\n\n";

    std::cout << "Relações de saída (->):\n";
    for (const Edge& e : g.outNeighbors(id)) {
        std::cout << "  --" << e.type << " [conf " << e.confidence << "]--> " << nameOf(g, e.neighborId)
                  << "\n";
    }
    std::cout << "\nRelações de entrada (<-):\n";
    for (const Edge& e : g.inNeighbors(id)) {
        std::cout << "  <--" << e.type << " [conf " << e.confidence << "]-- " << nameOf(g, e.neighborId)
                  << "\n";
    }
}

void demonstratePersistence(const Graph& original) {
    printHeader("Persistência em SQLite (Nível 2)");

    const std::string path = "grafo.db";
    std::cout << "A gravar o grafo em '" << path << "'...\n";
    {
        Database db(path);
        db.saveGraph(original);
        std::cout << "  Gravados " << db.countNodes() << " nós e " << db.countRelationships()
                  << " relações.\n";
    }

    std::cout << "A recarregar o grafo a partir da base de dados...\n";
    Database db(path);
    Graph reloaded = db.loadGraph();
    std::cout << "  Recarregados " << reloaded.nodeCount() << " nós e "
              << reloaded.relationshipCount() << " relações.\n\n";

    // Pesquisa ao nível da base de dados (por tipo e por relação).
    std::cout << "Pesquisa na BD: nós do tipo 'campaign':\n";
    for (const auto& n : db.findNodesByType("campaign")) {
        std::cout << "  - " << n.name << "\n";
    }
    std::cout << "\nPesquisa na BD: relações do tipo 'indicates':\n";
    for (const auto& r : db.findRelationships(std::nullopt, std::nullopt, std::string("indicates"))) {
        std::cout << "  " << nameOf(reloaded, r.source) << " --indicates--> "
                  << nameOf(reloaded, r.target) << "\n";
    }
}

void demonstrateAlgorithms(const Graph& g) {
    printHeader("Algoritmos de análise (Nível 3)");

    // Travessias BFS e DFS a partir de um nó (mostra a ordem de visita).
    auto printTraversal = [&](const std::string& title, const std::vector<std::string>& order) {
        std::cout << title;
        for (std::size_t i = 0; i < order.size() && i < 8; ++i)
            std::cout << (i ? " -> " : " ") << nameOf(g, order[i]);
        if (order.size() > 8) std::cout << " ...";
        std::cout << "\n";
    };
    std::cout << "Travessias a partir de APT29:\n";
    printTraversal("  BFS:", bfs(g, "threat-actor--apt29"));
    printTraversal("  DFS:", dfs(g, "threat-actor--apt29"));

    // PageRank: entidades mais relevantes.
    std::cout << "\n";
    std::cout << "Entidades mais influentes (PageRank, top 5):\n";
    auto ranks = pageRank(g);
    for (std::size_t i = 0; i < ranks.size() && i < 5; ++i) {
        std::cout << "  " << (i + 1) << ". " << nameOf(g, ranks[i].first) << "  (score "
                  << ranks[i].second << ")\n";
    }

    // Centralidade por grau: nós com mais ligações.
    std::cout << "\nCentralidade por grau (top 5 — entrada/saída/total):\n";
    auto degrees = degreeCentrality(g);
    for (std::size_t i = 0; i < degrees.size() && i < 5; ++i) {
        const auto& d = degrees[i];
        std::cout << "  " << (i + 1) << ". " << nameOf(g, d.id) << "  (in " << d.in << ", out "
                  << d.out << ", total " << d.total << ")\n";
    }

    // Caminho mais curto: do indicador suspeito até ao ator.
    std::cout << "\nCaminho entre um indicador e um ator:\n";
    Path p = shortestPath(g, "indicator--ip-185", "threat-actor--apt29");
    if (p.found) {
        std::cout << "  " << nameOf(g, p.nodes[0]);
        for (std::size_t i = 1; i < p.nodes.size(); ++i) {
            std::cout << " --" << p.edgeTypes[i - 1] << "--> " << nameOf(g, p.nodes[i]);
        }
        std::cout << "\n";
    } else {
        std::cout << "  (sem caminho)\n";
    }

    // Resolução de entidades.
    std::cout << "\nResolução de entidades (possíveis duplicados):\n";
    for (const auto& [a, b] : resolveEntities(g)) {
        std::cout << "  " << nameOf(g, a) << "  ~  " << nameOf(g, b)
                  << "  (provavelmente a mesma entidade)\n";
    }

    // Consulta analítica.
    std::cout << "\nQue atores usam o malware 'CozyDuke'?\n";
    for (const auto& id : actorsUsingMalware(g, "malware--cozyduke")) {
        std::cout << "  - " << nameOf(g, id) << "\n";
    }
}

void demonstrateValidation(Graph& g) {
    printHeader("Demonstração da validação de relações");

    struct Attempt {
        std::string id, source, target, type, descricao;
    };
    std::vector<Attempt> attempts = {
        {"demo--ok", "threat-actor--apt29", "malware--xagent", "uses",
         "VÁLIDA: threat-actor uses malware"},
        {"demo--bad-type", "malware--cozyduke", "threat-actor--apt29", "uses",
         "INVÁLIDA: malware uses threat-actor (não está na matriz)"},
        {"demo--missing", "threat-actor--apt29", "malware--inexistente", "uses",
         "INVÁLIDA: nó de destino não existe"},
    };

    for (const auto& a : attempts) {
        std::string error;
        bool ok = g.addRelationship(a.id, a.source, a.target, a.type, 50, &error);
        std::cout << "  " << a.descricao << "\n";
        std::cout << "    -> " << (ok ? "aceite" : "rejeitada: " + error) << "\n\n";
    }
}

}  // namespace

int main() {
    enableUtf8Console();

    std::cout << "Grafo de Conhecimento para Cyber Threat Intelligence\n";
    std::cout << "Nível 1 - Grafo básico em memória\n";

    Graph graph;
    loadSyntheticData(graph);

    showStatistics(graph);
    listNodesByType(graph, "threat-actor");
    listNodesByType(graph, "malware");
    showNeighbors(graph, "threat-actor--apt29");
    showNeighbors(graph, "malware--cozyduke");
    demonstrateAlgorithms(graph);
    demonstrateValidation(graph);
    demonstratePersistence(graph);

    std::cout << "\n";
    return 0;
}
