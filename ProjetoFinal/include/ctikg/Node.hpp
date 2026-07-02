#pragma once

#include <map>
#include <string>
#include <utility>

namespace ctikg {

// Um nó do grafo de conhecimento.
// Representa uma entidade de Cyber Threat Intelligence: um ator malicioso,
// uma peça de malware, um indicador de compromisso, uma vulnerabilidade, etc.
struct Node {
    std::string id;    // identificador único do nó (ex.: "threat-actor--apt29")
    std::string type;  // tipo de entidade, validado pelo Schema (ex.: "threat-actor")
    std::string name;  // nome legível (ex.: "APT29")

    // Atributos adicionais livres (ex.: "aliases" -> "Cozy Bear, The Dukes").
    std::map<std::string, std::string> properties;

    Node() = default;

    Node(std::string id, std::string type, std::string name)
        : id(std::move(id)), type(std::move(type)), name(std::move(name)) {}
};

}  // namespace ctikg
