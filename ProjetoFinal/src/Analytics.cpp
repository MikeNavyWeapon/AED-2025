#include "ctikg/Analytics.hpp"

#include <algorithm>
#include <cctype>
#include <functional>
#include <limits>
#include <queue>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace ctikg {

namespace {

// Arestas a seguir a partir de um nó: só de saída (dirigido) ou ambas.
std::vector<Edge> expand(const Graph& g, const std::string& id, bool directed) {
    return directed ? g.outNeighbors(id) : g.neighbors(id);
}

std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

std::string trim(const std::string& s) {
    std::size_t a = s.find_first_not_of(" \t");
    std::size_t b = s.find_last_not_of(" \t");
    if (a == std::string::npos) return "";
    return s.substr(a, b - a + 1);
}

// Separa "Cozy Bear, The Dukes" em {"cozy bear", "the dukes"} (em minúsculas).
std::vector<std::string> aliasTokens(const Node& n) {
    std::vector<std::string> tokens;
    auto it = n.properties.find("aliases");
    if (it == n.properties.end()) return tokens;
    std::stringstream ss(it->second);
    std::string item;
    while (std::getline(ss, item, ',')) {
        std::string t = toLower(trim(item));
        if (!t.empty()) tokens.push_back(t);
    }
    return tokens;
}

}  // namespace

// ----------------------------------------------------------------- travessias

std::vector<std::string> bfs(const Graph& g, const std::string& start, bool directed) {
    std::vector<std::string> order;
    if (!g.hasNode(start)) return order;

    std::unordered_set<std::string> visited{start};
    std::queue<std::string> q;
    q.push(start);
    while (!q.empty()) {
        std::string cur = q.front();
        q.pop();
        order.push_back(cur);
        for (const Edge& e : expand(g, cur, directed)) {
            if (visited.insert(e.neighborId).second) q.push(e.neighborId);
        }
    }
    return order;
}

std::vector<std::string> dfs(const Graph& g, const std::string& start, bool directed) {
    std::vector<std::string> order;
    if (!g.hasNode(start)) return order;

    std::unordered_set<std::string> visited;
    std::vector<std::string> stack{start};
    while (!stack.empty()) {
        std::string cur = stack.back();
        stack.pop_back();
        if (!visited.insert(cur).second) continue;  // já visitado
        order.push_back(cur);
        // Empilha vizinhos por ordem inversa para visitar na ordem natural.
        auto edges = expand(g, cur, directed);
        for (auto it = edges.rbegin(); it != edges.rend(); ++it) {
            if (!visited.count(it->neighborId)) stack.push_back(it->neighborId);
        }
    }
    return order;
}

// ------------------------------------------------------------ caminho mais curto

Path shortestPath(const Graph& g, const std::string& from, const std::string& to, bool directed) {
    Path path;
    if (!g.hasNode(from) || !g.hasNode(to)) return path;
    if (from == to) {
        path.found = true;
        path.nodes = {from};
        return path;
    }

    std::unordered_map<std::string, std::string> parent;      // nó -> nó anterior
    std::unordered_map<std::string, std::string> parentEdge;  // nó -> tipo da relação usada
    std::unordered_map<std::string, int> parentConf;          // nó -> confiança da aresta usada
    std::unordered_set<std::string> visited{from};
    std::queue<std::string> q;
    q.push(from);

    bool reached = false;
    while (!q.empty() && !reached) {
        std::string cur = q.front();
        q.pop();
        for (const Edge& e : expand(g, cur, directed)) {
            if (visited.insert(e.neighborId).second) {
                parent[e.neighborId] = cur;
                parentEdge[e.neighborId] = e.type;
                parentConf[e.neighborId] = e.confidence;
                if (e.neighborId == to) {
                    reached = true;
                    break;
                }
                q.push(e.neighborId);
            }
        }
    }

    if (!reached) return path;  // found continua false

    // Reconstrói o caminho do destino até à origem e inverte.
    std::vector<std::string> revNodes{to};
    std::vector<std::string> revEdges;
    std::vector<int> revConf;
    for (std::string at = to; at != from;) {
        revEdges.push_back(parentEdge[at]);
        revConf.push_back(parentConf[at]);
        at = parent[at];
        revNodes.push_back(at);
    }
    std::reverse(revNodes.begin(), revNodes.end());
    std::reverse(revEdges.begin(), revEdges.end());
    std::reverse(revConf.begin(), revConf.end());

    path.found = true;
    path.nodes = std::move(revNodes);
    path.edgeTypes = std::move(revEdges);
    path.edgeConfidences = std::move(revConf);
    return path;
}

// -------------------------------------------------- caminho de maior confiança

Path strongestPath(const Graph& g, const std::string& from, const std::string& to, bool directed) {
    Path path;
    if (!g.hasNode(from) || !g.hasNode(to)) return path;
    if (from == to) {
        path.found = true;
        path.nodes = {from};
        return path;
    }

    // Dijkstra: minimiza o custo total, onde custo(aresta) = 100 - confiança.
    const int INF = std::numeric_limits<int>::max();
    std::unordered_map<std::string, int> dist;
    std::unordered_map<std::string, std::string> parent, parentEdge;
    std::unordered_map<std::string, int> parentConf;
    for (const auto& [id, node] : g.nodes()) dist[id] = INF;
    dist[from] = 0;

    using Item = std::pair<int, std::string>;  // (custo acumulado, nó)
    std::priority_queue<Item, std::vector<Item>, std::greater<Item>> pq;
    pq.push({0, from});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (u == to) break;
        if (d > dist[u]) continue;  // entrada obsoleta
        for (const Edge& e : expand(g, u, directed)) {
            int w = 100 - e.confidence;  // confiança alta -> custo baixo
            int nd = d + w;
            if (nd < dist[e.neighborId]) {
                dist[e.neighborId] = nd;
                parent[e.neighborId] = u;
                parentEdge[e.neighborId] = e.type;
                parentConf[e.neighborId] = e.confidence;
                pq.push({nd, e.neighborId});
            }
        }
    }

    if (dist[to] == INF) return path;  // sem caminho

    std::vector<std::string> revNodes{to}, revEdges;
    std::vector<int> revConf;
    for (std::string at = to; at != from;) {
        revEdges.push_back(parentEdge[at]);
        revConf.push_back(parentConf[at]);
        at = parent[at];
        revNodes.push_back(at);
    }
    std::reverse(revNodes.begin(), revNodes.end());
    std::reverse(revEdges.begin(), revEdges.end());
    std::reverse(revConf.begin(), revConf.end());

    path.found = true;
    path.nodes = std::move(revNodes);
    path.edgeTypes = std::move(revEdges);
    path.edgeConfidences = std::move(revConf);
    return path;
}

// ------------------------------------------------------- centralidade por grau

std::vector<DegreeCentrality> degreeCentrality(const Graph& g) {
    std::vector<DegreeCentrality> result;
    result.reserve(g.nodeCount());
    for (const auto& [id, node] : g.nodes()) {
        DegreeCentrality d;
        d.id = id;
        d.out = static_cast<int>(g.outNeighbors(id).size());
        d.in = static_cast<int>(g.inNeighbors(id).size());
        d.total = d.in + d.out;
        result.push_back(d);
    }
    std::sort(result.begin(), result.end(),
              [](const DegreeCentrality& a, const DegreeCentrality& b) { return a.total > b.total; });
    return result;
}

// ------------------------------------------------------------------- PageRank

std::vector<std::pair<std::string, double>> pageRank(const Graph& g, double damping,
                                                     int iterations) {
    std::vector<std::pair<std::string, double>> result;
    const std::size_t n = g.nodeCount();
    if (n == 0) return result;

    // Indexa os nós e prepara grau de saída.
    std::vector<std::string> ids;
    ids.reserve(n);
    for (const auto& [id, node] : g.nodes()) ids.push_back(id);

    std::unordered_map<std::string, std::size_t> index;
    for (std::size_t i = 0; i < ids.size(); ++i) index[ids[i]] = i;

    std::vector<int> outDeg(n, 0);
    for (std::size_t i = 0; i < n; ++i) outDeg[i] = static_cast<int>(g.outNeighbors(ids[i]).size());

    const double base = (1.0 - damping) / static_cast<double>(n);
    std::vector<double> score(n, 1.0 / static_cast<double>(n));

    for (int iter = 0; iter < iterations; ++iter) {
        // Soma do peso dos nós sem arestas de saída (dangling).
        double dangling = 0.0;
        for (std::size_t i = 0; i < n; ++i)
            if (outDeg[i] == 0) dangling += score[i];

        std::vector<double> next(n, base + damping * dangling / static_cast<double>(n));

        for (std::size_t i = 0; i < n; ++i) {
            if (outDeg[i] == 0) continue;
            double share = score[i] / outDeg[i];
            for (const Edge& e : g.outNeighbors(ids[i])) {
                next[index[e.neighborId]] += damping * share;
            }
        }
        score.swap(next);
    }

    for (std::size_t i = 0; i < n; ++i) result.emplace_back(ids[i], score[i]);
    std::sort(result.begin(), result.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    return result;
}

std::vector<std::pair<std::string, double>> weightedPageRank(const Graph& g, double damping,
                                                             int iterations) {
    std::vector<std::pair<std::string, double>> result;
    const std::size_t n = g.nodeCount();
    if (n == 0) return result;

    std::vector<std::string> ids;
    ids.reserve(n);
    for (const auto& [id, node] : g.nodes()) ids.push_back(id);
    std::unordered_map<std::string, std::size_t> index;
    for (std::size_t i = 0; i < ids.size(); ++i) index[ids[i]] = i;

    // Peso total de saída de cada nó = soma das confianças das suas arestas.
    std::vector<double> outWeight(n, 0.0);
    for (std::size_t i = 0; i < n; ++i)
        for (const Edge& e : g.outNeighbors(ids[i])) outWeight[i] += e.confidence;

    const double base = (1.0 - damping) / static_cast<double>(n);
    std::vector<double> score(n, 1.0 / static_cast<double>(n));

    for (int iter = 0; iter < iterations; ++iter) {
        double dangling = 0.0;
        for (std::size_t i = 0; i < n; ++i)
            if (outWeight[i] == 0.0) dangling += score[i];

        std::vector<double> next(n, base + damping * dangling / static_cast<double>(n));

        for (std::size_t i = 0; i < n; ++i) {
            if (outWeight[i] == 0.0) continue;
            for (const Edge& e : g.outNeighbors(ids[i])) {
                // Fração proporcional à confiança desta aresta.
                double frac = e.confidence / outWeight[i];
                next[index[e.neighborId]] += damping * score[i] * frac;
            }
        }
        score.swap(next);
    }

    for (std::size_t i = 0; i < n; ++i) result.emplace_back(ids[i], score[i]);
    std::sort(result.begin(), result.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    return result;
}

// ----------------------------------------------------- deteção de comunidades

std::vector<std::vector<std::string>> connectedComponents(const Graph& g) {
    std::vector<std::vector<std::string>> components;
    std::unordered_set<std::string> seen;

    for (const auto& [id, node] : g.nodes()) {
        if (seen.count(id)) continue;
        // BFS não dirigido a partir deste nó forma uma componente.
        std::vector<std::string> component = bfs(g, id, /*directed=*/false);
        for (const auto& member : component) seen.insert(member);
        components.push_back(std::move(component));
    }
    // Ordena por tamanho decrescente (componentes maiores primeiro).
    std::sort(components.begin(), components.end(),
              [](const auto& a, const auto& b) { return a.size() > b.size(); });
    return components;
}

// --------------------------------------------------------- resolução de entidades

std::vector<std::pair<std::string, std::string>> resolveEntities(const Graph& g) {
    std::vector<std::pair<std::string, std::string>> pairs;

    std::vector<const Node*> nodes;
    for (const auto& [id, node] : g.nodes()) nodes.push_back(&node);

    for (std::size_t i = 0; i < nodes.size(); ++i) {
        for (std::size_t j = i + 1; j < nodes.size(); ++j) {
            const Node* a = nodes[i];
            const Node* b = nodes[j];
            if (a->type != b->type) continue;

            std::string nameA = toLower(trim(a->name));
            std::string nameB = toLower(trim(b->name));

            bool match = (nameA == nameB);
            if (!match) {
                auto aliasA = aliasTokens(*a);
                auto aliasB = aliasTokens(*b);
                // O nome de um aparece nos aliases do outro?
                match = std::find(aliasA.begin(), aliasA.end(), nameB) != aliasA.end() ||
                        std::find(aliasB.begin(), aliasB.end(), nameA) != aliasB.end();
            }
            if (match) pairs.emplace_back(a->id, b->id);
        }
    }
    return pairs;
}

// ----------------------------------------------------------- consultas analíticas

std::vector<std::string> actorsUsingMalware(const Graph& g, const std::string& malwareId) {
    std::vector<std::string> actors;
    for (const Edge& e : g.inNeighbors(malwareId)) {
        if (e.type != "uses") continue;
        const Node* src = g.getNode(e.neighborId);
        if (src && src->type == "threat-actor") actors.push_back(src->id);
    }
    return actors;
}

std::vector<std::string> indicatorsPointingTo(const Graph& g, const std::string& targetId) {
    std::vector<std::string> indicators;
    for (const Edge& e : g.inNeighbors(targetId)) {
        if (e.type != "indicates") continue;
        const Node* src = g.getNode(e.neighborId);
        if (src && src->type == "indicator") indicators.push_back(src->id);
    }
    return indicators;
}

}  // namespace ctikg
