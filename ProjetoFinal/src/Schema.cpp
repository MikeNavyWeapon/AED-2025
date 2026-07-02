#include "ctikg/Schema.hpp"

namespace ctikg {

Schema::Schema() {
    // ------------------------------------------------ tipos de entidade (nós)
    entityTypes_ = {
        "threat-actor",    // grupo ou indivíduo atacante (ex.: APT29)
        "malware",         // software malicioso (ex.: CozyDuke)
        "indicator",       // indicador de compromisso: IP, hash, domínio...
        "vulnerability",   // fraqueza explorável (ex.: CVE-2021-44228)
        "campaign",         // campanha de ataque com objetivo e período
        "identity",         // organização-alvo ou vítima
        "attack-pattern",   // técnica ou tática (ex.: spear phishing)
        "course-of-action", // medida defensiva / mitigação (ex.: MFA, patch)
    };

    // ------------------------------------------------------ tipos de relação
    relationTypes_ = {
        "uses",           // usa
        "indicates",      // indica / aponta para
        "targets",        // tem como alvo
        "exploits",       // explora
        "attributed-to",  // atribuído a
        "mitigates",      // mitiga
        "related-to",     // genérica
    };

    // ------------------------------------- matriz de relações válidas (triplos)
    // Triplos do enunciado:
    allowTriple("threat-actor", "uses", "malware");
    allowTriple("indicator", "indicates", "malware");
    allowTriple("campaign", "targets", "identity");
    allowTriple("malware", "uses", "attack-pattern");

    // Extensões coerentes para enriquecer o cenário de demonstração:
    allowTriple("threat-actor", "uses", "attack-pattern");
    allowTriple("threat-actor", "targets", "identity");
    allowTriple("campaign", "uses", "malware");
    allowTriple("campaign", "attributed-to", "threat-actor");
    allowTriple("malware", "exploits", "vulnerability");
    allowTriple("indicator", "indicates", "threat-actor");
    allowTriple("indicator", "indicates", "campaign");
    allowTriple("attack-pattern", "exploits", "vulnerability");
    allowTriple("threat-actor", "related-to", "threat-actor");

    // Lado defensivo: medidas que mitigam técnicas, malware ou vulnerabilidades.
    allowTriple("course-of-action", "mitigates", "attack-pattern");
    allowTriple("course-of-action", "mitigates", "malware");
    allowTriple("course-of-action", "mitigates", "vulnerability");
}

bool Schema::isEntityType(const std::string& type) const {
    return entityTypes_.count(type) > 0;
}

bool Schema::isRelationType(const std::string& type) const {
    return relationTypes_.count(type) > 0;
}

bool Schema::isValidRelation(const std::string& sourceType,
                             const std::string& relationType,
                             const std::string& targetType) const {
    return allowed_.count({sourceType, relationType, targetType}) > 0;
}

void Schema::allowEntityType(const std::string& type) { entityTypes_.insert(type); }

void Schema::allowRelationType(const std::string& type) { relationTypes_.insert(type); }

void Schema::allowTriple(const std::string& sourceType,
                         const std::string& relationType,
                         const std::string& targetType) {
    // Garante que os tipos referidos existem no esquema.
    entityTypes_.insert(sourceType);
    entityTypes_.insert(targetType);
    relationTypes_.insert(relationType);
    allowed_.insert({sourceType, relationType, targetType});
}

}  // namespace ctikg
