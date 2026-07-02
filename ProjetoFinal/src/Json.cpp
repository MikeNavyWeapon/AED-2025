#include "ctikg/Json.hpp"

#include <cmath>
#include <cstdio>
#include <sstream>
#include <stdexcept>

namespace ctikg {

namespace {

void escapeTo(std::ostream& os, const std::string& s) {
    os << '"';
    for (char c : s) {
        switch (c) {
            case '"': os << "\\\""; break;
            case '\\': os << "\\\\"; break;
            case '\n': os << "\\n"; break;
            case '\r': os << "\\r"; break;
            case '\t': os << "\\t"; break;
            default: os << c; break;
        }
    }
    os << '"';
}

void dumpTo(std::ostream& os, const Json& j);

void dumpTo(std::ostream& os, const Json& j) {
    switch (j.type()) {
        case Json::Type::Null:
            os << "null";
            break;
        case Json::Type::Bool:
            os << (j.asBool() ? "true" : "false");
            break;
        case Json::Type::Number: {
            double n = j.asNumber();
            if (n == std::floor(n) && std::fabs(n) < 1e15) {
                os << static_cast<long long>(n);  // inteiro "limpo"
            } else {
                char buf[32];
                std::snprintf(buf, sizeof(buf), "%g", n);
                os << buf;
            }
            break;
        }
        case Json::Type::String:
            escapeTo(os, j.asString());
            break;
        case Json::Type::Array: {
            os << '[';
            bool first = true;
            for (const auto& item : j.items()) {
                if (!first) os << ',';
                first = false;
                dumpTo(os, item);
            }
            os << ']';
            break;
        }
        case Json::Type::Object: {
            os << '{';
            bool first = true;
            for (const auto& [key, value] : j.members()) {
                if (!first) os << ',';
                first = false;
                escapeTo(os, key);
                os << ':';
                dumpTo(os, value);
            }
            os << '}';
            break;
        }
    }
}

// -------------------------------------------------------------------- parser
class Parser {
public:
    explicit Parser(const std::string& text) : s_(text) {}

    Json parse() {
        skipWs();
        Json result = parseValue();
        skipWs();
        if (pos_ != s_.size()) fail("texto a mais após o valor JSON");
        return result;
    }

private:
    const std::string& s_;
    std::size_t pos_ = 0;

    [[noreturn]] void fail(const std::string& msg) {
        throw std::runtime_error("JSON inválido: " + msg + " (posição " + std::to_string(pos_) + ")");
    }

    char peek() {
        if (pos_ >= s_.size()) fail("fim inesperado");
        return s_[pos_];
    }
    char get() {
        if (pos_ >= s_.size()) fail("fim inesperado");
        return s_[pos_++];
    }
    void skipWs() {
        while (pos_ < s_.size() &&
               (s_[pos_] == ' ' || s_[pos_] == '\t' || s_[pos_] == '\n' || s_[pos_] == '\r'))
            ++pos_;
    }
    void expect(char c) {
        if (get() != c) fail(std::string("esperado '") + c + "'");
    }

    Json parseValue() {
        skipWs();
        char c = peek();
        switch (c) {
            case '{': return parseObject();
            case '[': return parseArray();
            case '"': return Json(parseString());
            case 't':
            case 'f': return parseBool();
            case 'n': return parseNull();
            default: return parseNumber();
        }
    }

    Json parseObject() {
        Json obj = Json::object();
        expect('{');
        skipWs();
        if (peek() == '}') {
            get();
            return obj;
        }
        while (true) {
            skipWs();
            std::string key = parseString();
            skipWs();
            expect(':');
            obj[key] = parseValue();
            skipWs();
            char c = get();
            if (c == '}') break;
            if (c != ',') fail("esperado ',' ou '}'");
        }
        return obj;
    }

    Json parseArray() {
        Json arr = Json::array();
        expect('[');
        skipWs();
        if (peek() == ']') {
            get();
            return arr;
        }
        while (true) {
            arr.push_back(parseValue());
            skipWs();
            char c = get();
            if (c == ']') break;
            if (c != ',') fail("esperado ',' ou ']'");
        }
        return arr;
    }

    std::string parseString() {
        expect('"');
        std::string out;
        while (true) {
            char c = get();
            if (c == '"') break;
            if (c == '\\') {
                char e = get();
                switch (e) {
                    case '"': out += '"'; break;
                    case '\\': out += '\\'; break;
                    case '/': out += '/'; break;
                    case 'n': out += '\n'; break;
                    case 'r': out += '\r'; break;
                    case 't': out += '\t'; break;
                    case 'b': out += '\b'; break;
                    case 'f': out += '\f'; break;
                    case 'u': {
                        // Suporte simples: ignora o code point, mantém '?'.
                        for (int i = 0; i < 4; ++i) get();
                        out += '?';
                        break;
                    }
                    default: fail("escape inválido");
                }
            } else {
                out += c;
            }
        }
        return out;
    }

    Json parseBool() {
        if (s_.compare(pos_, 4, "true") == 0) {
            pos_ += 4;
            return Json(true);
        }
        if (s_.compare(pos_, 5, "false") == 0) {
            pos_ += 5;
            return Json(false);
        }
        fail("esperado true/false");
    }

    Json parseNull() {
        if (s_.compare(pos_, 4, "null") == 0) {
            pos_ += 4;
            return Json();
        }
        fail("esperado null");
    }

    Json parseNumber() {
        std::size_t start = pos_;
        if (peek() == '-') get();
        while (pos_ < s_.size() &&
               (std::isdigit(static_cast<unsigned char>(s_[pos_])) || s_[pos_] == '.' ||
                s_[pos_] == 'e' || s_[pos_] == 'E' || s_[pos_] == '+' || s_[pos_] == '-'))
            ++pos_;
        if (pos_ == start) fail("número inválido");
        return Json(std::stod(s_.substr(start, pos_ - start)));
    }
};

}  // namespace

std::string Json::dump() const {
    std::ostringstream os;
    dumpTo(os, *this);
    return os.str();
}

Json Json::parse(const std::string& text) {
    Parser p(text);
    return p.parse();
}

}  // namespace ctikg
