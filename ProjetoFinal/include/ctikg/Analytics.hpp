#pragma once

#include <string>
#include <utility>
#include <vector>

#include "ctikg/Graph.hpp"

namespace ctikg {

// Resultado de uma procura de caminho entre dois nós.
struct Path {
    bool found = false;
    std::vector<std::string> nodes;      // ids dos nós, do início ao fim
    std::vector<std::string> edgeTypes;  // relação entre nós consecutivos (tamanho = nós - 1)
    std::vector<int> edgeConfidences;    // confiança de cada aresta (tamanho = nós - 1)
};

// ----------------------------------------------------------------- travessias
// Por omissão tratam o grafo como NÃO dirigido (seguem arestas de saída e de
// entrada), o que é o mais útil para explorar ligações em CTI. Com directed=true
// seguem apenas as arestas de saída.
std::vector<std::string> bfs(const Graph& g, const std::string& start, bool directed = false);
std::vector<std::string> dfs(const Graph& g, const std::string& start, bool directed = false);

// Caminho mais curto (em número de arestas) via BFS. found=false se não existir.
Path shortestPath(const Graph& g, const std::string& from, const std::string& to,
                  bool directed = false);

// Caminho de MAIOR confiança via Dijkstra, usando peso = (100 - confiança) em
// cada aresta. Prefere ligações mais fiáveis, mesmo que use mais arestas.
Path strongestPath(const Graph& g, const std::string& from, const std::string& to,
                   bool directed = false);

// ------------------------------------------------------- centralidade por grau
// Grau de cada nó: relações de entrada, de saída e total. É a medida de
// centralidade mais simples — quantas ligações tem cada entidade.
struct DegreeCentrality {
    std::string id;
    int in = 0;
    int out = 0;
    int total = 0;
};

// Devolve a centralidade por grau de todos os nós, ordenada por total decrescente.
std::vector<DegreeCentrality> degreeCentrality(const Graph& g);

// ------------------------------------------------------------------- PageRank
// Devolve pares (idNó, score) ordenados por score decrescente. Usa as arestas
// dirigidas (de saída) e trata nós sem saída (dangling) distribuindo o seu peso.
std::vector<std::pair<std::string, double>> pageRank(const Graph& g, double damping = 0.85,
                                                     int iterations = 50);

// PageRank PONDERADO: a importância propaga-se proporcionalmente à confiança de
// cada aresta (em vez de igualmente por todas as arestas de saída).
std::vector<std::pair<std::string, double>> weightedPageRank(const Graph& g, double damping = 0.85,
                                                             int iterations = 50);

// ----------------------------------------------------- deteção de comunidades
// Componentes (fracamente) conexas: grupos de nós ligados entre si, ignorando a
// direção das relações. Uma forma simples de "deteção de comunidades". Cada
// componente é a lista de ids dos seus nós.
std::vector<std::vector<std::string>> connectedComponents(const Graph& g);

// --------------------------------------------------------- resolução de entidades
// Devolve pares de ids de nós que provavelmente representam a MESMA entidade
// (mesmo tipo + correspondência entre nome e aliases). Ex.: APT29 ~ Cozy Bear.
std::vector<std::pair<std::string, std::string>> resolveEntities(const Graph& g);

// ----------------------------------------------------------- consultas analíticas
// "Que atores usam determinado malware?"
std::vector<std::string> actorsUsingMalware(const Graph& g, const std::string& malwareId);

// "Que indicadores apontam para determinada entidade?"
std::vector<std::string> indicatorsPointingTo(const Graph& g, const std::string& targetId);

}  // namespace ctikg
