#pragma once

#include <istream>
#include <optional>
#include <sstream>
#include <variant>
#include <vector>

struct SymbolToken {
        std::string name;
        bool operator==(const SymbolToken& other) const;
};

struct QuoteToken {
        bool operator==(const QuoteToken&) const;
};

struct DotToken {
        bool operator==(const DotToken&) const;
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
        int64_t value;

        bool operator==(const ConstantToken& other) const;
        void FromString(const std::string& str);
};

struct BooleanToken {
        bool value;
        bool operator==(const BooleanToken& other) const;
};

struct InvalidToken {
        bool operator==(const InvalidToken& other) const;
};

using Token = std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken,
                           DotToken, BooleanToken, InvalidToken>;

std::vector<Token> Read(const std::string& string);

class Tokenizer {
    public:
        Tokenizer(std::istream* in);
        bool IsEnd();
        void Next();
        Token GetToken();

    private:
        void SkipEmpty();
        std::string ReadToken();

    private:
        std::istream* in_;
        Token current_token_;
};

template <typename T>
bool IsSameToken(Token* token) {
    return std::get_if<T>(token);
}
