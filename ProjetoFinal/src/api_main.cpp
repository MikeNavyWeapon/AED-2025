// Servidor da API REST do grafo de conhecimento (Nível 4).
// Assenta no GraphService (camada de serviço) e nas rotas de ApiRoutes.
//
// Uso:  ctikg_api [porta] [caminho_bd]
//   - porta:      por omissão 8080
//   - caminho_bd: se indicado, carrega o grafo dessa base de dados;
//                 caso contrário, usa os dados sintéticos.

#include <filesystem>
#include <iostream>
#include <string>

#include "ctikg/ApiRoutes.hpp"
#include "ctikg/GraphService.hpp"
#include "httplib.h"

using namespace ctikg;

namespace {

// Procura a pasta da interface web e monta-a como ficheiros estáticos.
// Tenta "web" (relativo ao diretório de trabalho) e, se não existir, o caminho
// absoluto fixado em compilação (CTIKG_WEB_DIR), para funcionar a partir de
// qualquer diretório.
bool mountWeb(httplib::Server& svr) {
    std::vector<std::string> candidates = {"web"};
#ifdef CTIKG_WEB_DIR
    candidates.push_back(CTIKG_WEB_DIR);
#endif
    for (const auto& dir : candidates) {
        if (std::filesystem::exists(dir) && svr.set_mount_point("/", dir)) {
            std::cout << "Interface web servida a partir de: " << dir << "\n";
            return true;
        }
    }
    std::cout << "(Sem pasta 'web' — apenas a API está disponível.)\n";
    return false;
}

}  // namespace

int main(int argc, char** argv) {
    int port = (argc > 1) ? std::stoi(argv[1]) : 8080;

    GraphService service;
    if (argc > 2) {
        try {
            service.loadFromDatabase(argv[2]);
            std::cout << "Grafo carregado de '" << argv[2] << "'.\n";
        } catch (const std::exception& ex) {
            std::cerr << "Erro ao carregar BD: " << ex.what() << "\n";
            return 1;
        }
    } else {
        service.loadSyntheticData();
        std::cout << "Dados sintéticos carregados.\n";
    }

    httplib::Server svr;
    registerApiRoutes(svr, service);
    mountWeb(svr);

    std::cout << "API a escutar em http://127.0.0.1:" << port << "\n";
    std::cout << "Interface: abre http://127.0.0.1:" << port << "/ no browser\n";
    std::cout << "Exemplos:\n";
    std::cout << "  curl http://127.0.0.1:" << port << "/stats\n";
    std::cout << "  curl http://127.0.0.1:" << port << "/nodes?type=threat-actor\n";
    std::cout << "Ctrl+C para terminar.\n";

    if (!svr.listen("127.0.0.1", port)) {
        std::cerr << "Não foi possível iniciar o servidor na porta " << port << ".\n";
        return 1;
    }
    return 0;
}
