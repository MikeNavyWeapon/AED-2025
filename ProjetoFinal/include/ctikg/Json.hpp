#pragma once

#include <map>
#include <string>
#include <vector>

namespace ctikg {

// JSON mínimo e auto-contido (sem dependências externas).
// Suporta os tipos necessários à API: null, bool, número, string, array e
// objeto. Objetos usam std::map para gerar saída determinística (chaves
// ordenadas), o que facilita testes.
class Json {
public:
    enum class Type { Null, Bool, Number, String, Array, Object };

    Json() : type_(Type::Null) {}
    Json(bool b) : type_(Type::Bool), bool_(b) {}
    Json(int n) : type_(Type::Number), num_(n) {}
    Json(long long n) : type_(Type::Number), num_(static_cast<double>(n)) {}
    Json(std::size_t n) : type_(Type::Number), num_(static_cast<double>(n)) {}
    Json(double n) : type_(Type::Number), num_(n) {}
    Json(const char* s) : type_(Type::String), str_(s) {}
    Json(std::string s) : type_(Type::String), str_(std::move(s)) {}

    static Json array() { Json j; j.type_ = Type::Array; return j; }
    static Json object() { Json j; j.type_ = Type::Object; return j; }

    Type type() const { return type_; }
    bool isObject() const { return type_ == Type::Object; }
    bool isArray() const { return type_ == Type::Array; }
    bool isString() const { return type_ == Type::String; }
    bool isNull() const { return type_ == Type::Null; }

    // Acesso/escrita em objeto (cria a chave se não existir).
    Json& operator[](const std::string& key) {
        type_ = Type::Object;
        return obj_[key];
    }
    bool contains(const std::string& key) const {
        return type_ == Type::Object && obj_.count(key) > 0;
    }

    // Acrescenta a um array.
    void push_back(Json value) {
        type_ = Type::Array;
        arr_.push_back(std::move(value));
    }

    // Acessores com valor por omissão.
    std::string asString(const std::string& def = "") const {
        return type_ == Type::String ? str_ : def;
    }
    double asNumber(double def = 0.0) const { return type_ == Type::Number ? num_ : def; }
    bool asBool(bool def = false) const { return type_ == Type::Bool ? bool_ : def; }

    std::string getString(const std::string& key, const std::string& def = "") const {
        auto it = obj_.find(key);
        return (type_ == Type::Object && it != obj_.end()) ? it->second.asString(def) : def;
    }

    const std::vector<Json>& items() const { return arr_; }
    const std::map<std::string, Json>& members() const { return obj_; }

    // Serializa para texto JSON.
    std::string dump() const;

    // Faz parse de texto JSON. Lança std::runtime_error em caso de erro.
    static Json parse(const std::string& text);

private:
    Type type_;
    bool bool_ = false;
    double num_ = 0.0;
    std::string str_;
    std::vector<Json> arr_;
    std::map<std::string, Json> obj_;
};

}  // namespace ctikg
