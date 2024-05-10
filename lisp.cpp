#include "lisp.h"
#include "heap.h"
#include "parser.h"
#include "tokenizer.h"

#include <cassert>

Interpreter::Interpreter() {
    scope_ = As<Scope>(Heap::GetHeap().Allocate<Scope>());
}

std::string Interpreter::Run(const std::string& str) {
    try {
        std::istringstream in(str);
        Tokenizer tokenizer(&in);

        auto root = Read(&tokenizer);

        if (!tokenizer.IsEnd()) {
            throw SyntaxError("Bad operation");
        }

        if (root == nullptr) {
            throw RuntimeError("No operations");
        }

        auto result = root->Eval(scope_);
        auto res = result->ToString();
        ClearUnused();
        return res;
    } catch (...) {
        ClearUnused();
        throw;
    }
}

Interpreter::~Interpreter() {
    Heap::GetHeap().DeleteUnmarked();
}

void Interpreter::ClearUnused() {
    scope_->Mark();
    Heap::GetHeap().DeleteUnmarked();
}
