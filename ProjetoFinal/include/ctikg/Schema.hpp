#pragma once

#include <set>
#include <string>
#include <tuple>

namespace ctikg {

// O Schema define as regras do grafo de conhecimento:
//   - que tipos de entidade (nós) existem;
//   - que tipos de relação existem;
//   - que combinações (origem, relação, destino) são permitidas.
//
// A "matriz de relações válidas" pedida no enunciado é representada aqui pelo
// conjunto de triplos permitidos. Exemplos do enunciado:
//   threat-actor uses     malware
//   indicator    indicates malware
//   campaign     targets   identity
//   malware      uses      attack-pattern
class Schema {
public:
    // Constrói o Schema com o esquema por omissão (tipos e matriz pré-definidos).
    Schema();

    bool isEntityType(const std::string& type) const;
    bool isRelationType(const std::string& type) const;

    // O triplo (tipo de origem, tipo de relação, tipo de destino) é permitido?
    bool isValidRelation(const std::string& sourceType,
                         const std::string& relationType,
                         const std::string& targetType) const;

    // Permite estender o esquema em tempo de execução.
    void allowEntityType(const std::string& type);
    void allowRelationType(const std::string& type);
    void allowTriple(const std::string& sourceType,
                     const std::string& relationType,
                     const std::string& targetType);

    const std::set<std::string>& entityTypes() const { return entityTypes_; }
    const std::set<std::string>& relationTypes() const { return relationTypes_; }
    const std::set<std::tuple<std::string, std::string, std::string>>& allowedTriples() const {
        return allowed_;
    }

private:
    std::set<std::string> entityTypes_;
    std::set<std::string> relationTypes_;
    std::set<std::tuple<std::string, std::string, std::string>> allowed_;
};

}  // namespace ctikg
