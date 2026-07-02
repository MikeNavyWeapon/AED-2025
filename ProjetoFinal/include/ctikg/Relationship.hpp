#pragma once

#include <string>
#include <utility>

namespace ctikg {

// Uma relação dirigida entre dois nós: source --(type)--> target.
// Exemplo: threat-actor--apt29 --(uses)--> malware--cozyduke
struct Relationship {
    std::string id;       // identificador único da relação
    std::string source;   // id do nó de origem
    std::string target;   // id do nó de destino
    std::string type;     // tipo de relação, validado pelo Schema (ex.: "uses")
    int confidence = 50;  // grau de confiança na relação, 0..100 (omissão: 50)

    Relationship() = default;

    Relationship(std::string id, std::string source, std::string target, std::string type,
                 int confidence = 50)
        : id(std::move(id)),
          source(std::move(source)),
          target(std::move(target)),
          type(std::move(type)),
          confidence(confidence) {}
};

}  // namespace ctikg
