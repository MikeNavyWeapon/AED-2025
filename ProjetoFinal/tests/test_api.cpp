// Testes de integração da API REST (Nível 4).
// Arranca o servidor numa thread, numa porta livre do loopback, e usa um
// cliente HTTP para exercitar os endpoints de ponta a ponta.

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "ctikg/ApiRoutes.hpp"
#include "ctikg/GraphService.hpp"
#include "ctikg/Json.hpp"
#include "httplib.h"

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

}  // namespace

int main() {
    std::cout << "=== Testes de integração do Nível 4 (API REST) ===\n\n";

    GraphService service;
    service.loadSyntheticData();

    httplib::Server svr;
    registerApiRoutes(svr, service);

    int port = svr.bind_to_any_port("127.0.0.1");
    if (port <= 0) {
        std::cout << "  [FALHA] não foi possível abrir uma porta de loopback\n";
        return 1;
    }
    std::thread server_thread([&]() { svr.listen_after_bind(); });

    // Espera que o servidor fique pronto.
    for (int i = 0; i < 100 && !svr.is_running(); ++i)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    httplib::Client cli("127.0.0.1", port);
    cli.set_connection_timeout(2, 0);
    cli.set_read_timeout(2, 0);

    // 1) GET /stats
    if (auto res = cli.Get("/stats"); res) {
        check(res->status == 200, "GET /stats responde 200");
        Json j = Json::parse(res->body);
        check(j.contains("nodes"), "stats tem campo 'nodes'");
        check(j.members().at("nodes").asNumber() > 0, "stats reporta nós > 0");
    } else {
        check(false, "GET /stats não obteve resposta");
    }

    // 2) GET /nodes?type=threat-actor
    if (auto res = cli.Get("/nodes?type=threat-actor"); res) {
        check(res->status == 200, "GET /nodes?type=... responde 200");
        Json j = Json::parse(res->body);
        check(j.isArray() && !j.items().empty(), "lista de atores não vazia");
    } else {
        check(false, "GET /nodes não obteve resposta");
    }

    // 3) GET /nodes/{id}
    if (auto res = cli.Get("/nodes/threat-actor--apt29"); res) {
        check(res->status == 200, "GET /nodes/{id} responde 200");
        Json j = Json::parse(res->body);
        check(j.getString("id") == "threat-actor--apt29", "devolve o nó pedido");
    } else {
        check(false, "GET /nodes/{id} não obteve resposta");
    }

    // 4) GET /nodes/{id} inexistente -> 404
    if (auto res = cli.Get("/nodes/nao-existe"); res) {
        check(res->status == 404, "nó inexistente responde 404");
    } else {
        check(false, "GET nó inexistente não obteve resposta");
    }

    // 5) GET /nodes/{id}/neighbors
    if (auto res = cli.Get("/nodes/threat-actor--apt29/neighbors"); res) {
        check(res->status == 200, "GET vizinhos responde 200");
        Json j = Json::parse(res->body);
        check(j.isArray() && !j.items().empty(), "lista de vizinhos não vazia");
    } else {
        check(false, "GET vizinhos não obteve resposta");
    }

    // 6) POST /nodes válido -> 201
    if (auto res = cli.Post("/nodes", R"({"id":"malware--teste","type":"malware","name":"Teste"})",
                            "application/json");
        res) {
        check(res->status == 201, "POST /nodes válido responde 201");
    } else {
        check(false, "POST /nodes não obteve resposta");
    }

    // 7) POST /nodes inválido (tipo desconhecido) -> 400
    if (auto res = cli.Post("/nodes", R"({"id":"x--1","type":"dragao","name":"X"})",
                            "application/json");
        res) {
        check(res->status == 400, "POST /nodes com tipo inválido responde 400");
    } else {
        check(false, "POST /nodes inválido não obteve resposta");
    }

    // 8) POST /relationships válido -> 201
    if (auto res = cli.Post(
            "/relationships",
            R"({"id":"rel--teste","source":"threat-actor--apt29","target":"malware--teste","type":"uses"})",
            "application/json");
        res) {
        check(res->status == 201, "POST /relationships válido responde 201");
    } else {
        check(false, "POST /relationships não obteve resposta");
    }

    // 9) GET /path
    if (auto res = cli.Get("/path?from=indicator--ip-185&to=threat-actor--apt29"); res) {
        check(res->status == 200, "GET /path responde 200");
        Json j = Json::parse(res->body);
        check(j.members().at("found").asBool(), "encontra caminho entre indicador e ator");
    } else {
        check(false, "GET /path não obteve resposta");
    }

    // 10) GET /ranking?top=3
    if (auto res = cli.Get("/ranking?top=3"); res) {
        check(res->status == 200, "GET /ranking responde 200");
        Json j = Json::parse(res->body);
        check(j.isArray() && j.items().size() == 3, "ranking devolve top 3");
    } else {
        check(false, "GET /ranking não obteve resposta");
    }

    // 11) GET /centrality?top=3 (centralidade por grau)
    if (auto res = cli.Get("/centrality?top=3"); res) {
        check(res->status == 200, "GET /centrality responde 200");
        Json j = Json::parse(res->body);
        check(j.isArray() && j.items().size() == 3, "centralidade devolve top 3");
        check(j.items()[0].contains("total"), "centralidade inclui grau total");
    } else {
        check(false, "GET /centrality não obteve resposta");
    }

    // 12) GET /relationships?type=indicates (listagem com filtro)
    if (auto res = cli.Get("/relationships?type=indicates"); res) {
        check(res->status == 200, "GET /relationships responde 200");
        Json j = Json::parse(res->body);
        check(j.isArray() && !j.items().empty(), "lista relações por tipo");
        check(j.items()[0].contains("confidence"), "relação inclui confiança");
    } else {
        check(false, "GET /relationships não obteve resposta");
    }

    // 12b) PUT /nodes/{id} -> atualiza (o malware--teste criado antes)
    if (auto res = cli.Put("/nodes/malware--teste", R"({"name":"Teste Editado"})", "application/json"); res) {
        check(res->status == 200, "PUT /nodes/{id} responde 200");
        Json j = Json::parse(res->body);
        check(j.getString("name") == "Teste Editado", "nome foi atualizado");
    } else {
        check(false, "PUT /nodes não obteve resposta");
    }

    // 12c) GET /strongest-path -> caminho de maior confiança
    if (auto res = cli.Get("/strongest-path?from=indicator--ip-185&to=threat-actor--apt29"); res) {
        check(res->status == 200, "GET /strongest-path responde 200");
        Json j = Json::parse(res->body);
        check(j.members().at("found").asBool(), "encontra caminho mais fiável");
        check(j.contains("minConfidence"), "inclui a confiança mínima do percurso");
    } else {
        check(false, "GET /strongest-path não obteve resposta");
    }

    // 13) GET /schema
    if (auto res = cli.Get("/schema"); res) {
        check(res->status == 200, "GET /schema responde 200");
        Json j = Json::parse(res->body);
        check(j.contains("entityTypes") && j.contains("relationTypes"), "schema tem tipos");
    } else {
        check(false, "GET /schema não obteve resposta");
    }

    // 14) DELETE /nodes/{id} (o malware--teste criado antes) -> 200
    if (auto res = cli.Delete("/nodes/malware--teste"); res) {
        check(res->status == 200, "DELETE /nodes/{id} responde 200");
    } else {
        check(false, "DELETE /nodes não obteve resposta");
    }

    // 15) POST /admin/reset -> repõe o dataset
    if (auto res = cli.Post("/admin/reset", "", "application/json"); res) {
        check(res->status == 200, "POST /admin/reset responde 200");
        Json j = Json::parse(res->body);
        check(j.members().at("nodes").asNumber() > 0, "reset repõe nós");
    } else {
        check(false, "POST /admin/reset não obteve resposta");
    }

    svr.stop();
    server_thread.join();

    std::cout << "\n--------------------------------------------\n";
    std::cout << "Verificações: " << g_checks << "  |  Falhas: " << g_failures << "\n";
    if (g_failures == 0) {
        std::cout << "RESULTADO: TODOS OS TESTES PASSARAM.\n";
        return 0;
    }
    std::cout << "RESULTADO: EXISTEM TESTES A FALHAR.\n";
    return 1;
}
