#include "ctikg/SyntheticData.hpp"

namespace ctikg {

void loadSyntheticData(Graph& graph) {
    // ----------------------------------------------------------------- nós
    // Atores
    Node apt29("threat-actor--apt29", "threat-actor", "APT29");
    apt29.properties["aliases"] = "Cozy Bear, The Dukes";
    apt29.properties["country"] = "RU";
    graph.addNode(apt29);

    Node cozyBear("threat-actor--cozy-bear", "threat-actor", "Cozy Bear");
    cozyBear.properties["aliases"] = "APT29";
    graph.addNode(cozyBear);  // duplicado proposital -> resolução de entidades (Nível 3)

    graph.addNode("threat-actor--apt28", "threat-actor", "APT28");

    // Malware
    graph.addNode("malware--cozyduke", "malware", "CozyDuke");
    graph.addNode("malware--wellmess", "malware", "WellMess");
    graph.addNode("malware--xagent", "malware", "X-Agent");

    // Indicadores (IoCs)
    graph.addNode("indicator--ip-185", "indicator", "185.86.148.13");
    graph.addNode("indicator--hash-a1b2", "indicator", "a1b2c3d4e5f6 (SHA-256)");
    graph.addNode("indicator--domain-evil", "indicator", "updates-microsoft[.]net");

    // Vulnerabilidades
    graph.addNode("vulnerability--cve-2021-44228", "vulnerability", "CVE-2021-44228 (Log4Shell)");
    graph.addNode("vulnerability--cve-2019-19781", "vulnerability", "CVE-2019-19781 (Citrix)");

    // Campanhas
    graph.addNode("campaign--solarwinds", "campaign", "Operação SolarWinds");
    graph.addNode("campaign--vaccine", "campaign", "Espionagem a investigação de vacinas");

    // Identidades (alvos)
    graph.addNode("identity--gov-agency", "identity", "Agência Governamental");
    graph.addNode("identity--pharma", "identity", "Laboratório Farmacêutico");

    // Técnicas (attack-pattern)
    graph.addNode("attack-pattern--spearphishing", "attack-pattern", "Spear Phishing");
    graph.addNode("attack-pattern--supplychain", "attack-pattern", "Supply Chain Compromise");

    // Medidas defensivas (course-of-action)
    graph.addNode("course-of-action--mfa", "course-of-action", "Autenticação multifator (MFA)");
    graph.addNode("course-of-action--patch-log4j", "course-of-action", "Aplicar patch Log4Shell");
    graph.addNode("course-of-action--patch-citrix", "course-of-action", "Aplicar patch Citrix");

    // -------------------------------------------------------------- relações
    // APT29 usa malware e técnicas (com graus de confiança variados)
    graph.addRelationship("rel--001", "threat-actor--apt29", "malware--cozyduke", "uses", 95);
    graph.addRelationship("rel--002", "threat-actor--apt29", "malware--wellmess", "uses", 80);
    graph.addRelationship("rel--003", "threat-actor--apt29", "attack-pattern--spearphishing", "uses");
    graph.addRelationship("rel--004", "threat-actor--apt29", "attack-pattern--supplychain", "uses");

    // APT28 usa o seu malware
    graph.addRelationship("rel--005", "threat-actor--apt28", "malware--xagent", "uses");

    // Indicadores apontam para malware (hash é a evidência mais forte)
    graph.addRelationship("rel--006", "indicator--ip-185", "malware--cozyduke", "indicates", 70);
    graph.addRelationship("rel--007", "indicator--hash-a1b2", "malware--cozyduke", "indicates", 99);
    graph.addRelationship("rel--008", "indicator--domain-evil", "malware--wellmess", "indicates", 85);

    // Malware explora vulnerabilidades e usa técnicas
    graph.addRelationship("rel--009", "malware--cozyduke", "vulnerability--cve-2021-44228", "exploits");
    graph.addRelationship("rel--010", "malware--wellmess", "vulnerability--cve-2019-19781", "exploits");
    graph.addRelationship("rel--011", "malware--cozyduke", "attack-pattern--spearphishing", "uses");

    // Campanhas: alvo, malware usado e atribuição
    graph.addRelationship("rel--012", "campaign--solarwinds", "identity--gov-agency", "targets");
    graph.addRelationship("rel--013", "campaign--solarwinds", "malware--cozyduke", "uses");
    graph.addRelationship("rel--014", "campaign--solarwinds", "threat-actor--apt29", "attributed-to", 75);
    graph.addRelationship("rel--015", "campaign--vaccine", "identity--pharma", "targets");
    graph.addRelationship("rel--016", "campaign--vaccine", "malware--wellmess", "uses");
    graph.addRelationship("rel--017", "campaign--vaccine", "threat-actor--apt29", "attributed-to");

    // Atores têm como alvo identidades
    graph.addRelationship("rel--018", "threat-actor--apt29", "identity--gov-agency", "targets");

    // Ligação para resolução de entidades (APT29 ~ Cozy Bear)
    graph.addRelationship("rel--019", "threat-actor--apt29", "threat-actor--cozy-bear", "related-to");

    // Lado defensivo: medidas que mitigam técnicas e vulnerabilidades.
    graph.addRelationship("rel--020", "course-of-action--mfa", "attack-pattern--spearphishing", "mitigates");
    graph.addRelationship("rel--021", "course-of-action--patch-log4j", "vulnerability--cve-2021-44228", "mitigates");
    graph.addRelationship("rel--022", "course-of-action--patch-citrix", "vulnerability--cve-2019-19781", "mitigates");

    // Mais ligações para um grafo mais rico (X-Agent explora a vuln. Citrix; técnica explora Log4Shell).
    graph.addRelationship("rel--023", "malware--xagent", "vulnerability--cve-2019-19781", "exploits");
    graph.addRelationship("rel--024", "attack-pattern--supplychain", "vulnerability--cve-2021-44228", "exploits");
    graph.addRelationship("rel--025", "threat-actor--apt28", "attack-pattern--spearphishing", "uses");
}

}  // namespace ctikg
