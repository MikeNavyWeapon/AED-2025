// Testes do módulo JSON próprio (usado pela API).

#include <iostream>
#include <string>

#include "ctikg/Json.hpp"

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

void test_build_and_dump() {
    std::cout << "test_build_and_dump\n";
    Json obj = Json::object();
    obj["id"] = "n--1";
    obj["count"] = 3;
    obj["active"] = true;
    Json arr = Json::array();
    arr.push_back(Json("a"));
    arr.push_back(Json("b"));
    obj["tags"] = arr;

    // Chaves ordenadas (std::map) -> saída determinística.
    check(obj.dump() == R"({"active":true,"count":3,"id":"n--1","tags":["a","b"]})",
          "serialização determinística de objeto");
}

void test_parse_basic() {
    std::cout << "test_parse_basic\n";
    Json j = Json::parse(R"({"id":"x","n":42,"ok":false,"nada":null})");
    check(j.isObject(), "parse devolve objeto");
    check(j.getString("id") == "x", "lê string");
    check(j.members().at("n").asNumber() == 42, "lê número");
    check(j.members().at("ok").asBool() == false, "lê bool");
    check(j.members().at("nada").isNull(), "lê null");
}

void test_parse_nested() {
    std::cout << "test_parse_nested\n";
    Json j = Json::parse(R"({"a":{"b":[1,2,{"c":"d"}]}})");
    const Json& b = j.members().at("a").members().at("b");
    check(b.isArray(), "array aninhado");
    check(b.items().size() == 3, "tamanho do array");
    check(b.items()[2].getString("c") == "d", "objeto dentro de array");
}

void test_roundtrip() {
    std::cout << "test_roundtrip\n";
    const std::string original = R"({"list":[true,false,null,7],"name":"teste"})";
    Json j = Json::parse(original);
    check(j.dump() == original, "parse -> dump preserva o conteúdo");
}

void test_escaping() {
    std::cout << "test_escaping\n";
    Json obj = Json::object();
    obj["s"] = std::string("linha1\nentre \"aspas\"");
    std::string dumped = obj.dump();
    Json back = Json::parse(dumped);
    check(back.getString("s") == "linha1\nentre \"aspas\"", "escape/parse de caracteres especiais");
}

void test_errors() {
    std::cout << "test_errors\n";
    bool threw = false;
    try {
        Json::parse(R"({"a":})");  // valor em falta
    } catch (const std::exception&) {
        threw = true;
    }
    check(threw, "JSON malformado lança exceção");

    threw = false;
    try {
        Json::parse(R"({"a":1) extra)");
    } catch (const std::exception&) {
        threw = true;
    }
    check(threw, "texto a mais lança exceção");
}

}  // namespace

int main() {
    std::cout << "=== Testes do módulo JSON ===\n\n";

    test_build_and_dump();
    test_parse_basic();
    test_parse_nested();
    test_roundtrip();
    test_escaping();
    test_errors();

    std::cout << "\n--------------------------------------------\n";
    std::cout << "Verificações: " << g_checks << "  |  Falhas: " << g_failures << "\n";
    if (g_failures == 0) {
        std::cout << "RESULTADO: TODOS OS TESTES PASSARAM.\n";
        return 0;
    }
    std::cout << "RESULTADO: EXISTEM TESTES A FALHAR.\n";
    return 1;
}
