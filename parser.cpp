#include "parser.h"
#include "heap.h"

namespace {
Object* Add(Object* root, Object* next) {
    if (root == nullptr) {
        return next;
    } else if (!Is<Cell>(root)) {
        root = Heap::GetHeap().Allocate<Cell>(root);
        As<Cell>(root)->SetSecond(next);
        return root;
    } else if (!(As<Cell>(root)->GetSecond())) {
        As<Cell>(root)->SetSecond(next);
        return root;
    } else {
        As<Cell>(root)->SetSecond(Add(As<Cell>(root)->GetSecond(), next));
        return root;
    }
}

int Size(Object* root) {
    if (root == nullptr) {
        return 0;
    } else if (!Is<Cell>(root)) {
        return 1;
    } else {
        return 1 + Size(As<Cell>(root)->GetSecond());
    }
}
}  // namespace

Object* Read(Tokenizer* tokenizer, bool consider_all) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("Empty sequence");
    }

    Token token = tokenizer->GetToken();

    if (token == Token(BracketToken::OPEN)) {
        tokenizer->Next();
        return ReadList(tokenizer, consider_all);
    } else if (token == Token(BracketToken::CLOSE)) {
        throw SyntaxError("Closed bracket without corresponding open");
    } else if (IsSameToken<BooleanToken>(&token)) {
        tokenizer->Next();
        return Heap::GetHeap().Allocate<Boolean>(
            std::get<BooleanToken>(token).value);
    } else if (IsSameToken<QuoteToken>(&token)) {
        if (consider_all) {
            tokenizer->Next();
            return Heap::GetHeap().Allocate<Symbol>("quote");
        } else {
            auto res = Heap::GetHeap().Allocate<Cell>(
                Heap::GetHeap().Allocate<Symbol>("quote"));
            tokenizer->Next();
            auto next = Read(tokenizer, true);
            if (!Is<Cell>(next)) {
                next = Heap::GetHeap().Allocate<Cell>(next);
            }
            As<Symbol>(As<Cell>(res)->GetFirst())->AddArgc(Size(next));
            As<Cell>(res)->SetSecond(next);
            return res;
        }
    } else if (IsSameToken<DotToken>(&token)) {
        throw SyntaxError("Dot unexpected");
    } else if (IsSameToken<SymbolToken>(&token)) {
        tokenizer->Next();
        return Heap::GetHeap().Allocate<Symbol>(
            std::get<SymbolToken>(token).name);
    } else if (IsSameToken<ConstantToken>(&token)) {
        tokenizer->Next();
        return Heap::GetHeap().Allocate<Number>(
            std::get<ConstantToken>(token).value);
    }

    throw SyntaxError("Unknown token");
}

namespace {
void Null(Object* root) {
    if (root == nullptr) {
        return;
    } else {
        As<Symbol>(As<Cell>(root)->GetFirst())->SetArgc(0);
        Null(As<Cell>(root)->GetSecond());
    }
}
}  // namespace

Object* ReadList(Tokenizer* tokenizer, bool consider_all) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("Open bracket without corresponding closed");
    }

    Object* root = nullptr;
    int argc = 0;

    Token token = tokenizer->GetToken();
    if (IsSameToken<DotToken>(&token)) {
        throw SyntaxError("Dot unexpected");
    }

    while (token != Token(BracketToken::CLOSE)) {
        if (tokenizer->IsEnd()) {
            throw SyntaxError("No closed bracket at the end");
        }
        if (token == Token{SymbolToken{"lambda"}}) {
            tokenizer->Next();
            auto lambda_info = Heap::GetHeap().Allocate<LambdaSymbol>("lambda");
            auto lambda = Heap::GetHeap().Allocate<LambdaCell>(nullptr);

            if (tokenizer->GetToken() == Token(BracketToken::CLOSE) ||
                tokenizer->IsEnd()) {
                throw SyntaxError("lambda only name");
            }

            auto args = Read(tokenizer);
            int sz = Size(args);
            As<LambdaSymbol>(lambda_info)->SetVarc(sz);

            if (tokenizer->GetToken() == Token(BracketToken::CLOSE) ||
                tokenizer->IsEnd()) {
                throw SyntaxError("lambda needs body");
            }

            Null(args);

            while (tokenizer->GetToken() != Token(BracketToken::CLOSE)) {
                if (tokenizer->IsEnd()) {
                    throw SyntaxError("No ) at the end");
                }

                auto expr = Read(tokenizer);

                if (!Is<Cell>(expr)) {
                    expr = Heap::GetHeap().Allocate<Cell>(expr);
                }

                args = Add(args, expr);
                sz += 1;
            }

            tokenizer->Next();

            As<LambdaSymbol>(lambda_info)->AddArgc(sz);
            As<LambdaCell>(lambda)->SetFirst(lambda_info);
            As<LambdaCell>(lambda)->SetSecond(args);

            return Heap::GetHeap().Allocate<Cell>(lambda);
        } else if (token == Token{SymbolToken{"define"}}) {
            auto define = Heap::GetHeap().Allocate<Cell>(
                Heap::GetHeap().Allocate<Symbol>("_define-var"));
            root = Add(root, define);

            tokenizer->Next();
            if (tokenizer->GetToken() == Token(BracketToken::CLOSE) ||
                tokenizer->IsEnd()) {
                throw SyntaxError("define expects 2 arguments");
            }

            if (tokenizer->GetToken() == Token(BracketToken::OPEN)) {
                As<Symbol>(As<Cell>(root)->GetFirst())->AddArgc(2);
                auto lambda_info =
                    Heap::GetHeap().Allocate<LambdaSymbol>("lambda");

                auto args = Read(tokenizer);
                Null(args);
                auto name =
                    Heap::GetHeap().Allocate<Cell>(As<Cell>(args)->GetFirst());
                args = As<Cell>(args)->GetSecond();
                root = Add(root, name);
                int sz = Size(args);
                As<LambdaSymbol>(lambda_info)->SetVarc(sz);

                if (tokenizer->GetToken() == Token(BracketToken::CLOSE) ||
                    tokenizer->IsEnd()) {
                    throw SyntaxError("lambda needs body");
                }

                while (tokenizer->GetToken() != Token(BracketToken::CLOSE)) {
                    if (tokenizer->IsEnd()) {
                        throw SyntaxError("No ) at the end");
                    }

                    auto expr = Read(tokenizer);

                    if (!Is<Cell>(expr)) {
                        expr = Heap::GetHeap().Allocate<Cell>(expr);
                    }

                    args = Add(args, expr);
                    sz += 1;
                }

                tokenizer->Next();

                As<LambdaSymbol>(lambda_info)->AddArgc(sz);
                auto lambda = Heap::GetHeap().Allocate<LambdaCell>(nullptr);

                As<LambdaCell>(lambda)->SetFirst(lambda_info);
                As<LambdaCell>(lambda)->SetSecond(args);

                return Add(root, lambda);
            }

            auto symbol = Read(tokenizer);
            if (Is<Cell>(symbol)) {
                throw SyntaxError(
                    "define does not expect expression as variable");
            }

            symbol = Heap::GetHeap().Allocate<Cell>(symbol);
            root = Add(root, symbol);

            if (tokenizer->GetToken() == Token(BracketToken::CLOSE) ||
                tokenizer->IsEnd()) {
                throw SyntaxError("define expects 2 arguments");
            }

            auto expr = Read(tokenizer);
            if (!Is<Cell>(expr)) {
                expr = Heap::GetHeap().Allocate<Cell>(expr);
            } else if (Is<Cell>(expr) &&
                       Is<LambdaCell>(As<Cell>(expr)->GetFirst())) {
                expr = As<Cell>(expr)->GetFirst();
            }
            root = Add(root, expr);

            if (tokenizer->GetToken() != Token(BracketToken::CLOSE)) {
                throw SyntaxError("define expects 2 arguments");
            }

            argc += 3;
        } else if (token == Token{SymbolToken{"set!"}}) {
            auto define = Heap::GetHeap().Allocate<Cell>(
                Heap::GetHeap().Allocate<Symbol>("_set-var"));
            root = Add(root, define);

            tokenizer->Next();
            if (tokenizer->GetToken() == Token(BracketToken::CLOSE) ||
                tokenizer->IsEnd()) {
                throw SyntaxError("set! expects 2 arguments");
            }

            auto symbol = Read(tokenizer);
            if (Is<Cell>(symbol)) {
                throw SyntaxError(
                    "set! does not expect expression as variable");
            }

            symbol = Heap::GetHeap().Allocate<Cell>(symbol);
            root = Add(root, symbol);

            if (tokenizer->GetToken() == Token(BracketToken::CLOSE) ||
                tokenizer->IsEnd()) {
                throw SyntaxError("set! expects 2 arguments");
            }

            auto expr = Read(tokenizer);
            if (!Is<Cell>(expr)) {
                expr = Heap::GetHeap().Allocate<Cell>(expr);
            }
            root = Add(root, expr);

            if (tokenizer->GetToken() != Token(BracketToken::CLOSE)) {
                throw SyntaxError("set! expects 2 arguments");
            }

            argc += 3;
        } else if (IsSameToken<DotToken>(&token)) {
            tokenizer->Next();

            auto expr = Read(tokenizer);

            if (tokenizer->GetToken() != Token(BracketToken::CLOSE)) {
                throw SyntaxError("Expected only 1 expression after .");
            }
            if (Is<Number>(expr)) {
                expr = Heap::GetHeap().Allocate<FakeNumber>(
                    As<Number>(expr)->GetValue());
            }

            root = Add(root, expr);
            argc++;
        } else if (IsSameToken<QuoteToken>(&token) ||
                   token == Token{SymbolToken{"quote"}}) {
            auto expr = Read(tokenizer, true);
            expr = Heap::GetHeap().Allocate<Cell>(expr);
            argc++;

            token = tokenizer->GetToken();
            if (token == Token(BracketToken::CLOSE) || tokenizer->IsEnd()) {
                throw SyntaxError("Expected sequence after quote");
            }

            auto next = Read(tokenizer, true);
            if (!Is<Cell>(next)) {
                next = Heap::GetHeap().Allocate<Cell>(next);
            }
            As<Symbol>(As<Cell>(expr)->GetFirst())->AddArgc(Size(next));
            root = Add(root, expr);
            root = Add(root, next);
        } else {
            auto expr = Read(tokenizer, consider_all);

            if (!Is<Cell>(expr)) {
                expr = Heap::GetHeap().Allocate<Cell>(expr);
            }
            root = Add(root, expr);
            argc++;
        }

        token = tokenizer->GetToken();
    }

    tokenizer->Next();

    if (Is<Cell>(root) && Is<Symbol>(As<Cell>(root)->GetFirst())) {
        argc--;
    }

    if (consider_all) {
        auto temp = root;
        root = Heap::GetHeap().Allocate<Cell>(
            Heap::GetHeap().Allocate<Symbol>("("));
        As<Cell>(root)->SetSecond(temp);
        root = Add(root, Heap::GetHeap().Allocate<Cell>(
                             Heap::GetHeap().Allocate<Symbol>(")")));
        argc += 2;
    } else if (Is<Cell>(root) && Is<Symbol>(As<Cell>(root)->GetFirst())) {
        As<Symbol>(As<Cell>(root)->GetFirst())->AddArgc(argc);
    }

    return root;
}
