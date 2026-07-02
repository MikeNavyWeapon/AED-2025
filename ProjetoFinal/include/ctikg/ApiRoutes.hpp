#pragma once

#include "ctikg/GraphService.hpp"
#include "httplib.h"

namespace ctikg {

// Regista todas as rotas da API REST no servidor, ligadas ao serviço dado.
// Está separada do main para poder ser usada também nos testes de integração.
void registerApiRoutes(httplib::Server& server, GraphService& service);

}  // namespace ctikg
