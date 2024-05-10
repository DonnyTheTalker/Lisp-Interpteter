#include "tokenizer.h"
#include "error.h"

#include <cassert>

namespace {

std::string blank_symbols = {EOF, ' ', '\t', '\n'};
std::string valid_start_symbols = "<=>*/#";
std::string valid_symbols = "<=>*/#?!-";

std::string mono_tokens = "().'";

bool IsNumber(const std::string &s) {
    if (s.empty()) {
        return false;
    }

    size_t start = s[0] == '+' || s[0] == '-';

    if (start && s.size() == 1) {
        return false;
    }

    return s.find_first_not_of("0123456789", start) == std::string::npos;
}

bool ValidStartSymbol(char ch) {
    return isalpha(ch) || valid_start_symbols.find(ch) != std::string::npos;
}

bool ValidSymbol(char ch) {
    return isalnum(ch) || valid_symbols.find(ch) != std::string::npos;
}

bool CheckValidSymbol(const std::string &s) {
    if (s.empty()) {
        return false;
    }

    if (s == "+" || s == "-") {
        return true;
    }

    if (!ValidStartSymbol(s[0])) {
        return false;
    }

    for (size_t i = 1; i < s.size(); i++) {
        if (!ValidSymbol(s[i])) {
            return false;
        }
    }

    return true;
}
}  // namespace

bool SymbolToken::operator==(const SymbolToken &other) const {
    return name == other.name;
}

bool QuoteToken::operator==(const QuoteToken &) const {
    return true;
}

bool DotToken::operator==(const DotToken &) const {
    return true;
}

bool ConstantToken::operator==(const ConstantToken &other) const {
    return value == other.value;
}

bool BooleanToken::operator==(const BooleanToken &other) const {
    return value == other.value;
}

bool InvalidToken::operator==(const InvalidToken &) const {
    return true;
}

void ConstantToken::FromString(const std::string &str) {
    assert(!str.empty());

    size_t start = str[0] == '-' || str[0] == '+';
    value = 0;

    for (size_t i = start; i != str.size(); ++i) {
        value = value * 10 + (str[i] - '0');
    }

    if (str[0] == '-') {
        value *= -1;
    }
}

std::vector<Token> Read(const std::string &string) {
    std::istringstream in(string);
    Tokenizer tokenizer(&in);

    std::vector<Token> result;
    while (!tokenizer.IsEnd()) {
        result.push_back(std::move(tokenizer.GetToken()));
        tokenizer.Next();
    }

    return result;
}

Tokenizer::Tokenizer(std::istream *in) : in_(in) {
    Next();
}

bool Tokenizer::IsEnd() {
    return IsSameToken<InvalidToken>(&current_token_);
}

void Tokenizer::Next() {
    SkipEmpty();

    std::string token = ReadToken();
    if (token.empty()) {
        current_token_ = InvalidToken();
    } else if (token == "'") {
        current_token_ = QuoteToken();
    } else if (token == ".") {
        current_token_ = DotToken();
    } else if (token == "(") {
        current_token_ = BracketToken::OPEN;
    } else if (token == ")") {
        current_token_ = BracketToken::CLOSE;
    } else if (IsNumber(token)) {
        ConstantToken temp;
        temp.FromString(token);
        current_token_ = temp;
    } else if (token.size() == 2 && token[0] == '#' && (token[1] == 'f' || token[1] == 't')) {
        BooleanToken temp;
        temp.value = token == "#t";
        current_token_ = temp;
    } else {
        if (CheckValidSymbol(token)) {
            SymbolToken temp;
            temp.name = token;
            current_token_ = temp;
        } else {
            throw SyntaxError("Invalid sequence: " + token);
        }
    }
}

Token Tokenizer::GetToken() {
    if (IsEnd()) {
        throw SyntaxError("None token is held");
    }

    return current_token_;
}

void Tokenizer::SkipEmpty() {
    while (in_->peek() != EOF) {
        char ch = in_->peek();
        if (blank_symbols.find(ch) != std::string::npos) {
            in_->get(ch);
        } else {
            break;
        }
    }
}

std::string Tokenizer::ReadToken() {
    std::string token;

    if (in_->peek() == EOF) {
        return token;
    }

    char start;
    in_->get(start);
    token.push_back(start);

    if (mono_tokens.find(start) != std::string::npos) {
        return token;
    }

    if (start == '+' || start == '-' || isdigit(start)) {
        while (isdigit(in_->peek())) {
            token.push_back(' ');
            in_->get(token.back());
        }

        return token;
    }

    if (start == '#' && (in_->peek() == 'f' || in_->peek() == 't')) {
        token.push_back(' ');
        in_->get(token.back());
        return token;
    }

    if (ValidStartSymbol(start)) {
        while (ValidSymbol(in_->peek())) {
            token.push_back(' ');
            in_->get(token.back());
        }

        return token;
    }

    while (blank_symbols.find(in_->peek()) == std::string::npos) {
        token.push_back(' ');
        in_->get(token.back());
    }

    return token;
}
