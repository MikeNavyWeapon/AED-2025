#include "ctikg/ApiRoutes.hpp"

#include <string>

#include "ctikg/Json.hpp"

namespace ctikg {

namespace {

// ----------------------------------------------------- serialização para JSON

Json nodeToJson(const Node& n) {
    Json j = Json::object();
    j["id"] = n.id;
    j["type"] = n.type;
    j["name"] = n.name;
    Json props = Json::object();
    for (const auto& [k, v] : n.properties) props[k] = v;
    j["properties"] = props;
    return j;
}

Json edgeToJson(const GraphService& svc, const Edge& e) {
    Json j = Json::object();
    j["relationshipId"] = e.relationshipId;
    j["type"] = e.type;
    j["direction"] = std::string(e.outgoing ? "out" : "in");
    j["confidence"] = e.confidence;
    j["neighborId"] = e.neighborId;
    if (const Node* n = svc.getNode(e.neighborId)) j["neighborName"] = n->name;
    return j;
}

Json relationshipToJson(const Relationship& r) {
    Json j = Json::object();
    j["id"] = r.id;
    j["source"] = r.source;
    j["target"] = r.target;
    j["type"] = r.type;
    j["confidence"] = r.confidence;
    return j;
}

Json pathToJson(const GraphService& svc, const Path& p) {
    Json j = Json::object();
    j["found"] = p.found;
    Json nodes = Json::array();
    for (const auto& id : p.nodes) {
        Json step = Json::object();
        step["id"] = id;
        if (const Node* n = svc.getNode(id)) step["name"] = n->name;
        nodes.push_back(step);
    }
    j["nodes"] = nodes;
    Json edges = Json::array();
    for (const auto& t : p.edgeTypes) edges.push_back(Json(t));
    j["edgeTypes"] = edges;
    Json confs = Json::array();
    int minc = 100;
    for (int c : p.edgeConfidences) {
        confs.push_back(Json(c));
        if (c < minc) minc = c;
    }
    j["edgeConfidences"] = confs;
    j["minConfidence"] = p.edgeConfidences.empty() ? 100 : minc;
    return j;
}

void sendError(httplib::Response& res, int status, const std::string& message) {
    Json j = Json::object();
    j["error"] = message;
    res.status = status;
    res.set_content(j.dump(), "application/json");
}

void sendJson(httplib::Response& res, const Json& j, int status = 200) {
    res.status = status;
    res.set_content(j.dump(), "application/json");
}

}  // namespace

void registerApiRoutes(httplib::Server& svr, GraphService& service) {
    // CORS: permite que a interface gráfica (Nível 5) chame a API pelo browser.
    svr.set_post_routing_handler([](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
    });

    // GET / -> informação simples.
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(
            "API do Grafo de Conhecimento CTI. Endpoints: /stats, /nodes, "
            "/nodes/{id}, /nodes/{id}/neighbors, /relationships, /path, /ranking, /events.",
            "text/plain");
    });

    // GET /graph -> grafo completo (nós + relações) para visualização.
    svr.Get("/graph", [&](const httplib::Request&, httplib::Response& res) {
        Json j = Json::object();
        Json nodes = Json::array();
        for (const auto& [id, node] : service.graph().nodes()) nodes.push_back(nodeToJson(node));
        j["nodes"] = nodes;
        Json rels = Json::array();
        for (const auto& r : service.graph().relationships()) rels.push_back(relationshipToJson(r));
        j["relationships"] = rels;
        sendJson(res, j);
    });

    // GET /schema -> tipos de entidade, tipos de relação e matriz de triplos.
    svr.Get("/schema", [&](const httplib::Request&, httplib::Response& res) {
        const Schema& s = service.graph().schema();
        Json j = Json::object();
        Json ents = Json::array();
        for (const auto& t : s.entityTypes()) ents.push_back(Json(t));
        j["entityTypes"] = ents;
        Json rels = Json::array();
        for (const auto& t : s.relationTypes()) rels.push_back(Json(t));
        j["relationTypes"] = rels;
        Json triples = Json::array();
        for (const auto& [src, rel, dst] : s.allowedTriples()) {
            Json t = Json::array();
            t.push_back(Json(src));
            t.push_back(Json(rel));
            t.push_back(Json(dst));
            triples.push_back(t);
        }
        j["triples"] = triples;
        sendJson(res, j);
    });

    // GET /stats -> estatísticas gerais.
    svr.Get("/stats", [&](const httplib::Request&, httplib::Response& res) {
        auto s = service.stats();
        Json j = Json::object();
        j["nodes"] = s.nodes;
        j["relationships"] = s.relationships;
        j["components"] = s.components;
        Json byType = Json::object();
        for (const auto& [type, count] : s.nodesByType) byType[type] = count;
        j["nodesByType"] = byType;
        sendJson(res, j);
    });

    // GET /nodes?type=...  -> lista nós por tipo.
    svr.Get("/nodes", [&](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_param("type")) {
            sendError(res, 400, "Parâmetro 'type' em falta. Use /nodes?type=threat-actor.");
            return;
        }
        Json arr = Json::array();
        for (const Node* n : service.nodesByType(req.get_param_value("type")))
            arr.push_back(nodeToJson(*n));
        sendJson(res, arr);
    });

    // POST /nodes -> cria um nó.  Body: {id, type, name, properties?}
    svr.Post("/nodes", [&](const httplib::Request& req, httplib::Response& res) {
        Json body;
        try {
            body = Json::parse(req.body);
        } catch (const std::exception& ex) {
            sendError(res, 400, ex.what());
            return;
        }
        std::map<std::string, std::string> props;
        if (body.contains("properties")) {
            for (const auto& [k, v] : body.members().at("properties").members())
                props[k] = v.asString();
        }
        auto result = service.createNode(body.getString("id"), body.getString("type"),
                                         body.getString("name"), props);
        if (!result.ok) {
            sendError(res, 400, result.error);
            return;
        }
        sendJson(res, nodeToJson(*service.getNode(body.getString("id"))), 201);
    });

    // GET /nodes/{id}/neighbors -> vizinhos de um nó (registar antes do genérico).
    svr.Get(R"(/nodes/([^/]+)/neighbors)", [&](const httplib::Request& req, httplib::Response& res) {
        std::string id = req.matches[1];
        if (!service.getNode(id)) {
            sendError(res, 404, "Nó inexistente: " + id);
            return;
        }
        Json arr = Json::array();
        for (const Edge& e : service.neighbors(id)) arr.push_back(edgeToJson(service, e));
        sendJson(res, arr);
    });

    // GET /nodes/{id} -> obtém um nó por identificador.
    svr.Get(R"(/nodes/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
        const Node* n = service.getNode(req.matches[1]);
        if (!n) {
            sendError(res, 404, "Nó inexistente: " + std::string(req.matches[1]));
            return;
        }
        sendJson(res, nodeToJson(*n));
    });

    // PUT /nodes/{id} -> atualiza o nome e/ou propriedades de um nó.
    svr.Put(R"(/nodes/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
        std::string id = req.matches[1];
        if (!service.getNode(id)) {
            sendError(res, 404, "Nó inexistente: " + id);
            return;
        }
        Json body;
        try {
            body = Json::parse(req.body);
        } catch (const std::exception& ex) {
            sendError(res, 400, ex.what());
            return;
        }
        std::map<std::string, std::string> props;
        if (body.contains("properties")) {
            for (const auto& [k, v] : body.members().at("properties").members())
                props[k] = v.asString();
        }
        auto result = service.updateNode(id, body.getString("name"), props);
        if (!result.ok) {
            sendError(res, 400, result.error);
            return;
        }
        sendJson(res, nodeToJson(*service.getNode(id)));
    });

    // DELETE /nodes/{id} -> remove um nó e as suas relações.
    svr.Delete(R"(/nodes/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
        auto result = service.deleteNode(req.matches[1]);
        if (!result.ok) {
            sendError(res, 404, result.error);
            return;
        }
        Json j = Json::object();
        j["status"] = std::string("deleted");
        sendJson(res, j);
    });

    // DELETE /relationships/{id} -> remove uma relação.
    svr.Delete(R"(/relationships/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
        auto result = service.deleteRelationship(req.matches[1]);
        if (!result.ok) {
            sendError(res, 404, result.error);
            return;
        }
        Json j = Json::object();
        j["status"] = std::string("deleted");
        sendJson(res, j);
    });

    // GET /relationships?source=&target=&type= -> lista relações (com filtros).
    svr.Get("/relationships", [&](const httplib::Request& req, httplib::Response& res) {
        std::optional<std::string> source, target, type;
        if (req.has_param("source")) source = req.get_param_value("source");
        if (req.has_param("target")) target = req.get_param_value("target");
        if (req.has_param("type")) type = req.get_param_value("type");
        Json arr = Json::array();
        for (const auto& r : service.relationships(source, target, type))
            arr.push_back(relationshipToJson(r));
        sendJson(res, arr);
    });

    // POST /relationships -> cria uma relação.  Body: {id, source, target, type, confidence?}
    svr.Post("/relationships", [&](const httplib::Request& req, httplib::Response& res) {
        Json body;
        try {
            body = Json::parse(req.body);
        } catch (const std::exception& ex) {
            sendError(res, 400, ex.what());
            return;
        }
        int confidence = body.contains("confidence")
                             ? static_cast<int>(body.members().at("confidence").asNumber(50))
                             : 50;
        auto result = service.createRelationship(body.getString("id"), body.getString("source"),
                                                 body.getString("target"), body.getString("type"),
                                                 confidence);
        if (!result.ok) {
            sendError(res, 400, result.error);
            return;
        }
        Json j = Json::object();
        j["status"] = std::string("created");
        sendJson(res, j, 201);
    });

    // GET /path?from=..&to=.. -> caminho mais curto entre dois nós.
    svr.Get("/path", [&](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_param("from") || !req.has_param("to")) {
            sendError(res, 400, "Parâmetros 'from' e 'to' são obrigatórios.");
            return;
        }
        std::string from = req.get_param_value("from");
        std::string to = req.get_param_value("to");
        if (!service.getNode(from) || !service.getNode(to)) {
            sendError(res, 404, "Origem ou destino inexistente.");
            return;
        }
        sendJson(res, pathToJson(service, service.shortestPath(from, to)));
    });

    // GET /strongest-path?from=..&to=.. -> caminho de MAIOR confiança (Dijkstra).
    svr.Get("/strongest-path", [&](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_param("from") || !req.has_param("to")) {
            sendError(res, 400, "Parâmetros 'from' e 'to' são obrigatórios.");
            return;
        }
        std::string from = req.get_param_value("from"), to = req.get_param_value("to");
        if (!service.getNode(from) || !service.getNode(to)) {
            sendError(res, 404, "Origem ou destino inexistente.");
            return;
        }
        sendJson(res, pathToJson(service, service.strongestPath(from, to)));
    });

    // GET /ranking?top=N -> entidades mais relevantes (PageRank).
    svr.Get("/ranking", [&](const httplib::Request& req, httplib::Response& res) {
        std::size_t top = req.has_param("top")
                              ? static_cast<std::size_t>(std::stoul(req.get_param_value("top")))
                              : 10;
        bool weighted = req.has_param("weighted") && req.get_param_value("weighted") == "true";
        auto ranks = weighted ? service.weightedRanking(top) : service.ranking(top);
        Json arr = Json::array();
        for (const auto& [id, score] : ranks) {
            Json item = Json::object();
            item["id"] = id;
            if (const Node* n = service.getNode(id)) item["name"] = n->name;
            item["score"] = score;
            arr.push_back(item);
        }
        sendJson(res, arr);
    });

    // GET /traverse?start=<id>&mode=bfs|dfs -> ordem de visita da travessia.
    svr.Get("/traverse", [&](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_param("start")) {
            sendError(res, 400, "Parâmetro 'start' é obrigatório.");
            return;
        }
        std::string start = req.get_param_value("start");
        if (!service.getNode(start)) {
            sendError(res, 404, "Nó inexistente: " + start);
            return;
        }
        std::string mode = req.has_param("mode") ? req.get_param_value("mode") : "bfs";
        std::vector<std::string> order =
            (mode == "dfs") ? dfs(service.graph(), start) : bfs(service.graph(), start);
        Json j = Json::object();
        j["mode"] = mode;
        Json arr = Json::array();
        for (const auto& id : order) {
            Json item = Json::object();
            item["id"] = id;
            if (const Node* n = service.getNode(id)) item["name"] = n->name;
            arr.push_back(item);
        }
        j["order"] = arr;
        sendJson(res, j);
    });

    // GET /centrality?top=N -> centralidade por grau (entrada/saída/total).
    svr.Get("/centrality", [&](const httplib::Request& req, httplib::Response& res) {
        std::size_t top = req.has_param("top")
                              ? static_cast<std::size_t>(std::stoul(req.get_param_value("top")))
                              : 10;
        Json arr = Json::array();
        for (const auto& d : service.degreeRanking(top)) {
            Json item = Json::object();
            item["id"] = d.id;
            if (const Node* n = service.getNode(d.id)) item["name"] = n->name;
            item["in"] = d.in;
            item["out"] = d.out;
            item["total"] = d.total;
            arr.push_back(item);
        }
        sendJson(res, arr);
    });

    // GET /events -> fila de eventos (bónus).
    svr.Get("/events", [&](const httplib::Request&, httplib::Response& res) {
        Json arr = Json::array();
        for (const auto& e : service.events()) arr.push_back(Json(e));
        sendJson(res, arr);
    });

    // --------------------------------------------------- administração / persistência
    auto countsJson = [&]() {
        Json j = Json::object();
        j["status"] = std::string("ok");
        j["nodes"] = service.graph().nodeCount();
        j["relationships"] = service.graph().relationshipCount();
        return j;
    };

    // POST /admin/reset -> repõe os dados sintéticos.
    svr.Post("/admin/reset", [&, countsJson](const httplib::Request&, httplib::Response& res) {
        service.resetToSynthetic();
        sendJson(res, countsJson());
    });

    // POST /admin/save  body {path?} -> grava o grafo na base de dados.
    svr.Post("/admin/save", [&, countsJson](const httplib::Request& req, httplib::Response& res) {
        std::string path = "grafo.db";
        if (!req.body.empty()) {
            try {
                Json b = Json::parse(req.body);
                if (b.contains("path")) path = b.getString("path");
            } catch (...) {
            }
        }
        try {
            service.saveToDatabase(path);
            Json j = countsJson();
            j["path"] = path;
            sendJson(res, j);
        } catch (const std::exception& ex) {
            sendError(res, 500, ex.what());
        }
    });

    // POST /admin/load  body {path?} -> carrega o grafo da base de dados.
    svr.Post("/admin/load", [&, countsJson](const httplib::Request& req, httplib::Response& res) {
        std::string path = "grafo.db";
        if (!req.body.empty()) {
            try {
                Json b = Json::parse(req.body);
                if (b.contains("path")) path = b.getString("path");
            } catch (...) {
            }
        }
        try {
            service.loadFromDatabase(path);
            Json j = countsJson();
            j["path"] = path;
            sendJson(res, j);
        } catch (const std::exception& ex) {
            sendError(res, 500, ex.what());
        }
    });
}

}  // namespace ctikg
