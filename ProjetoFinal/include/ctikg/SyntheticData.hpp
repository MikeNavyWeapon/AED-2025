#pragma once

#include "ctikg/Graph.hpp"

namespace ctikg {

// Preenche o grafo com um conjunto de dados sintéticos coerentes de CTI.
//
// O cenário foi desenhado para suportar a demonstração narrativa do enunciado:
// um indicador suspeito aponta para um malware, esse malware é usado por um
// ator, que por sua vez está associado a uma campanha contra uma organização.
void loadSyntheticData(Graph& graph);

}  // namespace ctikg
