#include "object.h"
#include "heap.h"
#include "error.h"

namespace {
bool IsArithmetic(const std::string& s) {
    std::string temp = "+-/*";
    return temp.find(s) != std::string ::npos && s.size() == 1;
}
}  // namespace

Number::Number(int64_t value) : value_(value) {
}

int64_t Number::GetValue() const {
    return value_;
}

Symbol::Symbol(const std::string& str) : name_(str) {
}

const std::string& Symbol::GetName() const {
    return name_;
}

void Symbol::AddArgc(int argc) {
    argc_ += argc;
}

void Symbol::SetArgc(int argc) {
    argc_ = argc;
}

bool Boolean::GetValue() const {
    return value_;
}

Boolean::Boolean(bool value) : value_(value) {
}

Cell::Cell(Object* first) : first_(first), second_(nullptr) {
}

Object* Cell::GetFirst() const {
    return first_;
}
Object* Cell::GetSecond() const {
    return second_;
}

void Cell::SetFirst(Object* ptr) {
    first_ = ptr;
}
void Cell::SetSecond(Object* ptr) {
    second_ = ptr;
}

Object* Number::Eval(Scope* scope) {
    return Heap::GetHeap().Allocate<Number>(value_);
}

std::string Number::ToString() {
    return std::to_string(value_);
}

Object* Symbol::Eval(Scope* scope) {
    auto alias = scope->Get(name_);
    if (alias != nullptr) {
        if (Is<Symbol>(alias) && IsArithmetic(As<Symbol>(alias)->GetName())) {
            auto s = As<Symbol>(alias)->GetName();

            if (s == "+") {
                return Heap::GetHeap().Allocate<Sum>(argc_);
            } else if (s == "-") {
                return Heap::GetHeap().Allocate<Difference>(argc_);
            } else if (s == "*") {
                return Heap::GetHeap().Allocate<Product>(argc_);
            } else if (s == "/") {
                return Heap::GetHeap().Allocate<Division>(argc_);
            }
        }

        return alias;
    }

    if (name_ == "quote") {
        return Heap::GetHeap().Allocate<QuoteFunction>(argc_);
    } else if (name_ == "number?") {
        return Heap::GetHeap().Allocate<IsNumber>(argc_);
    } else if (name_ == "boolean?") {
        return Heap::GetHeap().Allocate<IsBoolean>(argc_);
    } else if (name_ == "=") {
        return Heap::GetHeap().Allocate<Equal>(argc_);
    } else if (name_ == "<") {
        return Heap::GetHeap().Allocate<Less>(argc_);
    } else if (name_ == ">") {
        return Heap::GetHeap().Allocate<Greater>(argc_);
    } else if (name_ == "<=") {
        return Heap::GetHeap().Allocate<LessEqual>(argc_);
    } else if (name_ == ">=") {
        return Heap::GetHeap().Allocate<GreaterEqual>(argc_);
    } else if (name_ == "+") {
        return Heap::GetHeap().Allocate<Sum>(argc_);
    } else if (name_ == "-") {
        return Heap::GetHeap().Allocate<Difference>(argc_);
    } else if (name_ == "*") {
        return Heap::GetHeap().Allocate<Product>(argc_);
    } else if (name_ == "/") {
        return Heap::GetHeap().Allocate<Division>(argc_);
    } else if (name_ == "max") {
        return Heap::GetHeap().Allocate<Max>(argc_);
    } else if (name_ == "min") {
        return Heap::GetHeap().Allocate<Min>(argc_);
    } else if (name_ == "abs") {
        return Heap::GetHeap().Allocate<Abs>(argc_);
    } else if (name_ == "not") {
        return Heap::GetHeap().Allocate<Not>(argc_);
    } else if (name_ == "and") {
        return Heap::GetHeap().Allocate<And>(argc_);
    } else if (name_ == "or") {
        return Heap::GetHeap().Allocate<Or>(argc_);
    } else if (name_ == "pair?") {
        return Heap::GetHeap().Allocate<IsPair>(argc_);
    } else if (name_ == "null?") {
        return Heap::GetHeap().Allocate<IsNull>(argc_);
    } else if (name_ == "list?") {
        return Heap::GetHeap().Allocate<IsList>(argc_);
    } else if (name_ == "cons") {
        return Heap::GetHeap().Allocate<MakePair>(argc_);
    } else if (name_ == "car") {
        return Heap::GetHeap().Allocate<Head>(argc_);
    } else if (name_ == "cdr") {
        return Heap::GetHeap().Allocate<Tail>(argc_);
    } else if (name_ == "list") {
        return Heap::GetHeap().Allocate<MakeList>(argc_);
    } else if (name_ == "list-tail") {
        return Heap::GetHeap().Allocate<ListTail>(argc_);
    } else if (name_ == "list-ref") {
        return Heap::GetHeap().Allocate<ListRef>(argc_);
    } else if (name_ == "symbol?") {
        return Heap::GetHeap().Allocate<IsSymbol>(argc_);
    } else if (name_ == "_define-var") {
        return Heap::GetHeap().Allocate<DefineVar>(argc_);
    } else if (name_ == "_set-var") {
        return Heap::GetHeap().Allocate<SetVar>(argc_);
    } else if (name_ == "set-car!") {
        return Heap::GetHeap().Allocate<SetHead>(argc_);
    } else if (name_ == "set-cdr!") {
        return Heap::GetHeap().Allocate<SetTail>(argc_);
    } else if (name_ == "if") {
        return Heap::GetHeap().Allocate<If>(argc_);
    }

    throw NameError("no such name: " + name_);
}

std::string Symbol::ToString() {
    return name_;
}

Object* Boolean::Eval(Scope* scope) {
    return Heap::GetHeap().Allocate<Boolean>(value_);
}

std::string Boolean::ToString() {
    if (value_) {
        return "#t";
    }
    return "#f";
}

Object* Cell::Eval(Scope* scope) {
    if (GetFirst() == nullptr) {
        throw RuntimeError("No function provided");
    }

    if (!Is<Symbol>(GetFirst()) && !Is<LambdaInvoker>(GetFirst()) && !Is<LambdaCell>(GetFirst())) {
        throw RuntimeError("Wrong function provided");
    }

    auto func = GetFirst()->Eval(scope);

    if (Is<LambdaInvoker>(func) && Is<Symbol>(GetFirst())) {
        if (As<LambdaInvoker>(func)->GetArgc() != As<Symbol>(GetFirst())->GetArgc()) {
            auto args = As<LambdaInvoker>(func)->CollectArgs(GetSecond(), scope);
            auto res = As<LambdaInvoker>(func)->Apply(args, scope);

            if (!Is<LambdaInvoker>(res)) {
                throw RuntimeError("Wrong number of arguments");
            } else {
                args = As<LambdaInvoker>(res)->CollectArgs(GetSecond(), scope);
                return As<LambdaInvoker>(res)->Apply(args, scope);
            }
        }
    }

    if (Is<Function>(func)) {
        auto args = As<Function>(func)->CollectArgs(GetSecond(), scope);
        return As<Function>(func)->Apply(args, scope);
    } else {
        return func;
    }
}

std::string Cell::ToString() {
    throw RuntimeError("Something went wrong");
}

int Symbol::GetArgc() const {
    return argc_;
}

Function::Function(int argc) : argc_(argc) {
}

Object* Function::Eval(Scope* scope) {
    throw RuntimeError("Something wrong");
}

std::string Function::ToString() {
    throw RuntimeError("Something wrong");
}

namespace {
int GetArgc(Object* root, int cnt) {
    int ans = cnt;
    for (int i = 0; i < cnt; i++) {
        auto left = As<Cell>(root)->GetFirst();
        if (Is<Symbol>(left)) {
            int temp = GetArgc(As<Cell>(root)->GetSecond(), As<Symbol>(left)->GetArgc());
            ans += temp;
            for (int j = 0; j < temp; j++) {
                root = As<Cell>(root)->GetSecond();
            }
        }

        if (Is<Cell>(root)) {
            root = As<Cell>(root)->GetSecond();
        } else if (i + 1 != cnt) {
            throw RuntimeError("function no eval error");
        }
    }

    return ans;
}
}  // namespace

std::vector<Object*> FunctionNoEval::CollectArgs(Object* root, Scope* scope) {
    std::vector<Object*> result;

    try {
        for (int i = 0; i < argc_; i++) {
            if (!Is<Cell>(root) && !Is<LambdaCell>(root)) {
                throw RuntimeError("func no eval isn't cell");
            }

            if (Is<LambdaCell>(root)) {
                result.push_back(root);
            } else {
                auto left = As<Cell>(root)->GetFirst();
                if (Is<Symbol>(left)) {
                    result.push_back(root);
                    int temp = GetArgc(As<Cell>(root)->GetSecond(), As<Symbol>(left)->GetArgc());
                    for (int j = 0; j < temp; j++) {
                        root = As<Cell>(root)->GetSecond();
                    }
                } else {
                    result.push_back(As<Cell>(root)->GetFirst());
                }
            }

            if (Is<Cell>(root)) {
                root = As<Cell>(root)->GetSecond();
            } else if (i + 1 != argc_) {
                throw RuntimeError("function no eval error");
            }
        }
    } catch (...) {
        throw RuntimeError("unknown exception function no eval");
    }

    return result;
}

std::vector<Object*> FunctionEval::CollectArgs(Object* root, Scope* scope) {
    std::vector<Object*> result;

    try {
        for (int i = 0; i < argc_; i++) {
            if (!Is<Cell>(root)) {
                throw RuntimeError("func eval isn't cell");
            }

            auto left = As<Cell>(root)->GetFirst();
            if (Is<Symbol>(left)) {
                result.push_back(root->Eval(scope));
                int temp = GetArgc(As<Cell>(root)->GetSecond(), As<Symbol>(left)->GetArgc());
                for (int j = 0; j < temp; j++) {
                    root = As<Cell>(root)->GetSecond();
                }
            } else if (As<Cell>(root)->GetFirst() != nullptr) {
                result.push_back(As<Cell>(root)->GetFirst()->Eval(scope));
            } else {
                throw RuntimeError("Missing argument");
            }

            if (Is<Cell>(root)) {
                root = As<Cell>(root)->GetSecond();
            } else if (i + 1 != argc_) {
                throw RuntimeError("function eval error");
            }
        }
    } catch (...) {
        throw RuntimeError("function eval error unknown");
    }

    return result;
}

std::vector<Object*> QuoteFunction::CollectArgs(Object* root, Scope* scope) {
    std::vector<Object*> result;

    try {
        for (int i = 0; i < argc_; i++) {
            if (!Is<Cell>(root)) {
                throw RuntimeError("Something wrong");
            }
            result.push_back(As<Cell>(root)->GetFirst());
            root = As<Cell>(root)->GetSecond();
        }
    } catch (...) {
        throw RuntimeError("Something wrong");
    }

    return result;
}

Object* QuoteFunction::Apply(std::vector<Object*>& args, Scope* scope) {
    return Heap::GetHeap().Allocate<List>(args);
}

List::List(const std::vector<Object*>& state) : state_(state) {
}

std::string List::ToString() {
    std::string res;

    try {
        for (int i = 0; i < state_.size(); i++) {
            auto cur = state_[i];
            if (!cur) {
                throw RuntimeError("List to string");
            }

            if (Is<FakeNumber>(cur) && this->Size() > 1) {
                res += " . ";
                res += cur->ToString();
                continue;
            }

            if (i > 0 && cur->ToString() != ")" && res.back() != '(') {
                res += " ";
            }
            res += cur->ToString();
        }
    } catch (...) {
        throw RuntimeError("Something went wrong");
    }

    return res;
}

Object* List::Eval(Scope* scope) {
    return Heap::GetHeap().Allocate<List>(*this);
}

Object* IsNumber::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.size() != 1) {
        throw RuntimeError("number? invalid args");
    }

    return Heap::GetHeap().Allocate<Boolean>(Is<Number>(args[0]));
}

Object* IsBoolean::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.size() != 1) {
        throw RuntimeError("boolean? invalid args");
    }

    return Heap::GetHeap().Allocate<Boolean>(Is<Boolean>(args[0]));
}

namespace {
bool IsTrue(Object* obj) {
    return !Is<Boolean>(obj) || (Is<Boolean>(obj) && As<Boolean>(obj)->GetValue());
}
}  // namespace

Object* Not::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.size() != 1) {
        throw RuntimeError("not invalid args");
    }

    return Heap::GetHeap().Allocate<Boolean>(!IsTrue(args[0]));
}

Object* Equal::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.empty()) {
        return Heap::GetHeap().Allocate<Boolean>(true);
    }

    auto first = args[0];

    for (int i = 0; i < args.size(); i++) {
        auto cur = args[i];

        if (!Is<Number>(cur)) {
            throw RuntimeError("= invalid args");
        }

        if (As<Number>(first)->GetValue() != As<Number>(cur)->GetValue()) {
            return Heap::GetHeap().Allocate<Boolean>(false);
        }
    }

    return Heap::GetHeap().Allocate<Boolean>(true);
}

Object* Less::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.empty()) {
        return Heap::GetHeap().Allocate<Boolean>(true);
    }

    auto first = args[0];

    for (int i = 0; i < args.size(); i++) {
        auto cur = args[i];

        if (!Is<Number>(cur)) {
            throw RuntimeError("< invalid args");
        }

        if (i > 0 && As<Number>(first)->GetValue() >= As<Number>(cur)->GetValue()) {
            return Heap::GetHeap().Allocate<Boolean>(false);
        }
        first = cur;
    }

    return Heap::GetHeap().Allocate<Boolean>(true);
}

Object* Greater::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.empty()) {
        return Heap::GetHeap().Allocate<Boolean>(true);
    }

    auto first = args[0];

    for (int i = 0; i < args.size(); i++) {
        auto cur = args[i];

        if (!Is<Number>(cur)) {
            throw RuntimeError("> invalid args");
        }

        if (i > 0 && As<Number>(first)->GetValue() <= As<Number>(cur)->GetValue()) {
            return Heap::GetHeap().Allocate<Boolean>(false);
        }
        first = cur;
    }

    return Heap::GetHeap().Allocate<Boolean>(true);
}

Object* LessEqual::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.empty()) {
        return Heap::GetHeap().Allocate<Boolean>(true);
    }

    auto first = args[0];

    for (int i = 0; i < args.size(); i++) {
        auto cur = args[i];

        if (!Is<Number>(cur)) {
            throw RuntimeError("<= invalid args");
        }

        if (i > 0 && As<Number>(first)->GetValue() > As<Number>(cur)->GetValue()) {
            return Heap::GetHeap().Allocate<Boolean>(false);
        }
        first = cur;
    }

    return Heap::GetHeap().Allocate<Boolean>(true);
}

Object* GreaterEqual::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.empty()) {
        return Heap::GetHeap().Allocate<Boolean>(true);
    }

    auto first = args[0];

    for (int i = 0; i < args.size(); i++) {
        auto cur = args[i];

        if (!Is<Number>(cur)) {
            throw RuntimeError(">= invalid args");
        }

        if (i > 0 && As<Number>(first)->GetValue() < As<Number>(cur)->GetValue()) {
            return Heap::GetHeap().Allocate<Boolean>(false);
        }
        first = cur;
    }

    return Heap::GetHeap().Allocate<Boolean>(true);
}

Object* Sum::Apply(std::vector<Object*>& args, Scope* scope) {
    int64_t res = 0;

    for (size_t i = 0; i < args.size(); ++i) {
        auto cur = args[i];

        if (!Is<Number>(cur)) {
            throw RuntimeError("+ invalid args");
        }

        res += As<Number>(cur)->GetValue();
    }

    return Heap::GetHeap().Allocate<Number>(res);
}

Object* Difference::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.empty()) {
        throw RuntimeError("- expected argument");
    }

    int64_t res = 0;

    for (size_t i = 0; i < args.size(); ++i) {
        auto cur = args[i];

        if (!Is<Number>(cur)) {
            throw RuntimeError("- invalid args");
        }

        if (i == 0) {
            res += As<Number>(cur)->GetValue();
        } else {
            res -= As<Number>(cur)->GetValue();
        }
    }

    return Heap::GetHeap().Allocate<Number>(res);
}

Object* Product::Apply(std::vector<Object*>& args, Scope* scope) {
    int64_t res = 1;

    for (size_t i = 0; i < args.size(); ++i) {
        auto cur = args[i];

        if (!Is<Number>(cur)) {
            throw RuntimeError("* invalid args");
        }

        res *= As<Number>(cur)->GetValue();
    }

    return Heap::GetHeap().Allocate<Number>(res);
}

Object* Division::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.empty()) {
        throw RuntimeError("/ expected argument");
    }

    int64_t res = 0;

    for (size_t i = 0; i < args.size(); ++i) {
        auto cur = args[i];

        if (!Is<Number>(cur)) {
            throw RuntimeError("/ invalid args");
        }

        if (i == 0) {
            res += As<Number>(cur)->GetValue();
        } else {
            res /= As<Number>(cur)->GetValue();
        }
    }

    return Heap::GetHeap().Allocate<Number>(res);
}

Object* Max::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.empty()) {
        throw RuntimeError("max expected argument");
    }

    int64_t res = 0;

    for (size_t i = 0; i < args.size(); ++i) {
        auto cur = args[i];

        if (!Is<Number>(cur)) {
            throw RuntimeError("max invalid args");
        }

        if (i == 0) {
            res = As<Number>(cur)->GetValue();
        } else {
            res = std::max(res, As<Number>(cur)->GetValue());
        }
    }

    return Heap::GetHeap().Allocate<Number>(res);
}

Object* Min::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.empty()) {
        throw RuntimeError("min expected argument");
    }

    int64_t res = 0;

    for (size_t i = 0; i < args.size(); ++i) {
        auto cur = args[i];

        if (!Is<Number>(cur)) {
            throw RuntimeError("min invalid args");
        }

        if (i == 0) {
            res = As<Number>(cur)->GetValue();
        } else {
            res = std::min(res, As<Number>(cur)->GetValue());
        }
    }

    return Heap::GetHeap().Allocate<Number>(res);
}

Object* Abs::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.size() != 1) {
        throw RuntimeError("abs expects 1 argument");
    }

    auto arg = args[0];
    if (!Is<Number>(arg)) {
        throw RuntimeError("abs invalid argument");
    }

    return Heap::GetHeap().Allocate<Number>(std::abs(As<Number>(arg)->GetValue()));
}

Object* And::Apply(std::vector<Object*>& args, Scope* scope) {
    for (size_t i = 0; i < args.size(); i++) {
        auto cur = args[i]->Eval(scope);
        if (!IsTrue(cur)) {
            return cur;
        }
        if (i + 1 == args.size()) {
            return cur;
        }
    }
    return Heap::GetHeap().Allocate<Boolean>(true);
}

Object* Or::Apply(std::vector<Object*>& args, Scope* scope) {
    for (size_t i = 0; i < args.size(); i++) {
        auto cur = args[i]->Eval(scope);
        if (IsTrue(cur)) {
            return cur;
        }
        if (i + 1 == args.size()) {
            return cur;
        }
    }
    return Heap::GetHeap().Allocate<Boolean>(false);
}

bool List::IsObject(size_t i) {
    if (Is<Symbol>(state_[i])) {
        auto ch = As<Symbol>(state_[i])->GetName();
        if (ch == "(" || ch == ")") {
            return false;
        }
    }
    return true;
}

int List::Size() {
    int cnt = 0;
    for (int i = 0; i < state_.size(); i++) {
        cnt += IsObject(i);
    }
    return cnt;
}

Object* IsPair::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.size() != 1 || !Is<List>(args[0])) {
        return Heap::GetHeap().Allocate<Boolean>(false);
    }

    auto list = As<List>(args[0]);
    return Heap::GetHeap().Allocate<Boolean>(list->Size() == 2);
}

Object* IsNull::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.size() != 1 || !Is<List>(args[0])) {
        return Heap::GetHeap().Allocate<Boolean>(false);
    }

    auto list = As<List>(args[0]);
    return Heap::GetHeap().Allocate<Boolean>(list->Size() == 0);
}

Object* IsList::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.size() != 1 || !Is<List>(args[0])) {
        return Heap::GetHeap().Allocate<Boolean>(false);
    }

    auto list = As<List>(args[0]);
    return Heap::GetHeap().Allocate<Boolean>(!list->IsMalformed());
}

bool List::IsMalformed() {
    for (int i = 0; i < state_.size(); i++) {
        if (IsObject(i) && (Is<FakeNumber>(state_[i]) || !Is<Number>(state_[i]))) {
            return true;
        }
    }
    return false;
}

Object* MakePair::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.size() != 2) {
        throw RuntimeError("cons expects 2 arguments");
    }

    std::vector<Object*> list;
    list.reserve(4);

    list.push_back(Heap::GetHeap().Allocate<Symbol>("("));

    list.push_back(args[0]);

    if (Is<Number>(args[1])) {
        list.push_back(Heap::GetHeap().Allocate<FakeNumber>(As<Number>(args[1])->GetValue()));
    } else {
        list.push_back(args[1]);
    }

    list.push_back(Heap::GetHeap().Allocate<Symbol>(")"));
    return Heap::GetHeap().Allocate<List>(list);
}

Object* List::Get(size_t ind) {
    int cur = 0;

    for (int i = 0; i < state_.size(); i++) {
        if (IsObject(i)) {
            cur += 1;
        }
        if (cur == ind + 1) {
            return state_[i];
        }
    }

    return nullptr;
}

Object* Head::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.size() != 1) {
        throw RuntimeError("car expects 1 argument");
    }

    auto list = As<List>(args[0]);
    if (!list) {
        throw RuntimeError("car expects list");
    }

    if (list->Size() == 0) {
        throw RuntimeError("car expects none empty list");
    }

    return list->Get(0);
}

Object* Tail::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.size() != 1) {
        throw RuntimeError("cdr expects 1 argument");
    }

    auto list = As<List>(args[0]);
    if (!list) {
        throw RuntimeError("cdr expects list");
    }

    if (list->Size() == 0) {
        throw RuntimeError("cdr expects none empty list");
    }

    if (list->Size() == 2 && list->IsMalformed()) {
        return list->Get(1);
    }

    std::vector<Object*> new_list;
    new_list.push_back(Heap::GetHeap().Allocate<Symbol>("("));

    for (int i = 1; i < list->Size(); i++) {
        new_list.push_back(list->Get(i));
    }

    new_list.push_back(Heap::GetHeap().Allocate<Symbol>(")"));
    return Heap::GetHeap().Allocate<List>(new_list);
}

Object* MakeList::Apply(std::vector<Object*>& args, Scope* scope) {
    std::vector<Object*> list;
    list.reserve(args.size() + 2);

    list.push_back(Heap::GetHeap().Allocate<Symbol>("("));

    for (int i = 0; i < args.size(); i++) {
        auto cur = args[i];
        if (!Is<Number>(cur)) {
            throw RuntimeError("list expects numbers");
        }
        list.push_back(cur);
    }

    list.push_back(Heap::GetHeap().Allocate<Symbol>(")"));
    return Heap::GetHeap().Allocate<List>(list);
}

Object* ListRef::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.size() != 2) {
        throw RuntimeError("list-ref expects 2 arguments");
    }

    auto list = As<List>(args[0]);
    auto index = As<Number>(args[1]);

    if (!list || !index || list->Size() <= index->GetValue()) {
        throw RuntimeError("list-ref invalid arguments");
    }

    return list->Get(index->GetValue());
}

Object* ListTail::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.size() != 2) {
        throw RuntimeError("list-ref expects 2 arguments");
    }

    auto list = As<List>(args[0]);
    auto index = As<Number>(args[1]);

    if (!list || !index || list->Size() < index->GetValue()) {
        throw RuntimeError("list-ref invalid arguments");
    }

    std::vector<Object*> new_list;
    new_list.push_back(Heap::GetHeap().Allocate<Symbol>("("));

    for (int i = index->GetValue(); i < list->Size(); i++) {
        new_list.push_back(list->Get(i));
    }

    new_list.push_back(Heap::GetHeap().Allocate<Symbol>(")"));
    return Heap::GetHeap().Allocate<List>(new_list);
}

Object* IsSymbol::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.size() != 1 || !Is<List>(args[0])) {
        return Heap::GetHeap().Allocate<Boolean>(false);
    }

    auto list = As<List>(args[0]);
    return Heap::GetHeap().Allocate<Boolean>(list->Size() == 1 && Is<Symbol>(list->Get(0)));
}

Object* Scope::Get(const std::string& key) {
    if (map_.contains(key)) {
        return map_[key];
    }

    auto scope = prev_scope_;
    while (scope != nullptr && &(*scope) != this) {
        if (scope->map_.contains(key)) {
            return scope->map_[key];
        }
        scope = scope->prev_scope_;
    }

    return nullptr;
}

bool Scope::Add(const std::string& key, Object* obj, bool force_add) {
    auto temp = Get(key);
    if (temp == nullptr && force_add) {
        return false;
    }

    if (map_.contains(key)) {
        map_[key] = obj;
        return true;
    }

    auto scope = prev_scope_;
    while (scope != nullptr && &(*scope) != this) {
        if (scope->map_.contains(key)) {
            scope->map_[key] = obj;
            return true;
        }
        scope = scope->prev_scope_;
    }

    map_[key] = obj;
    return true;
}

Object* DefineVar::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.size() != 2) {
        throw RuntimeError("define expects 2 arguments");
    }

    auto symbol = As<Symbol>(As<Cell>(args[0])->GetFirst());
    auto expr = args[1];

    if (!symbol || !expr) {
        throw RuntimeError("define invalid arguments");
    }

    if (Is<Cell>(expr) && As<Cell>(expr)->GetSecond() == nullptr &&
        Is<Symbol>(As<Cell>(expr)->GetFirst()) &&
        IsArithmetic(As<Symbol>(As<Cell>(expr)->GetFirst())->GetName())) {
        return Heap::GetHeap().Allocate<Boolean>(
            scope->Add(symbol->GetName(), As<Cell>(expr)->GetFirst(), false));
    }

    return Heap::GetHeap().Allocate<Boolean>(
        scope->Add(symbol->GetName(), expr->Eval(scope), false));
}

Object* SetVar::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.size() != 2) {
        throw RuntimeError("set! expects 2 arguments");
    }

    auto symbol = As<Symbol>(As<Cell>(args[0])->GetFirst());
    auto expr = args[1];

    if (!symbol || !expr) {
        throw RuntimeError("set! invalid arguments");
    }

    auto res = scope->Add(symbol->GetName(), expr->Eval(scope), true);
    if (!res) {
        throw NameError("Variable " + symbol->GetName() + " not exist");
    }
    return Heap::GetHeap().Allocate<Boolean>(res);
}

void List::Set(size_t ind, Object* obj) {
    int cur = 0;

    for (int i = 0; i < state_.size(); i++) {
        if (IsObject(i)) {
            cur += 1;
        }
        if (cur == ind + 1) {
            state_[i] = obj;
            break;
        }
    }
}

Object* SetHead::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.size() != 2) {
        throw RuntimeError("set-car! expects 2 arguments");
    }

    auto list = As<List>(args[0]);
    if (!list) {
        throw RuntimeError("set-car! expects list as a 1 argument");
    }

    if (list->Size() == 0) {
        throw RuntimeError("set-car! expects none empty list");
    }

    list->Set(0, args[1]);
    return Heap::GetHeap().Allocate<Boolean>(true);
}

Object* SetTail::Apply(std::vector<Object*>& args, Scope* scope) {

    if (args.size() != 2) {
        throw RuntimeError("set-cdr! expects 2 arguments");
    }

    auto list = As<List>(args[0]);
    if (!list) {
        throw RuntimeError("set-cdr! expects list as a 1 argument");
    }

    if (list->Size() == 0) {
        throw RuntimeError("set-cdr! expects none empty list");
    }

    if (Is<Number>(args[1])) {
        args[1] = Heap::GetHeap().Allocate<FakeNumber>(As<Number>(args[1])->GetValue());
    }

    list->Set(1, args[1]);
    return Heap::GetHeap().Allocate<Boolean>(true);
}

Object* If::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.size() <= 1 || args.size() > 3) {
        throw SyntaxError("If wrong number of arguments");
    }

    bool res = IsTrue(args[0]->Eval(scope));
    if (res) {
        return args[1]->Eval(scope);
    }
    if (args.size() == 2) {
        std::vector<Object*> list;
        list.push_back(Heap::GetHeap().Allocate<Symbol>("("));
        list.push_back(Heap::GetHeap().Allocate<Symbol>(")"));
        return Heap::GetHeap().Allocate<List>(list);
    }

    return args[2]->Eval(scope);
}

int LambdaSymbol::GetVarc() const {
    return varc_;
}

void LambdaSymbol::SetVarc(int varc) {
    varc_ += varc;
}

std::string LambdaCell::ToString() {
    return "wrong";
}

LambdaCell::LambdaCell(Object* first) : first_(first), second_(nullptr) {
}

void LambdaCell::SetFirst(Object* ptr) {
    first_ = ptr;
}

void LambdaCell::SetSecond(Object* ptr) {
    second_ = ptr;
}

LambdaFunction::LambdaFunction(int argc, int argv, Scope* scope)
    : FunctionNoEval(argc), argv_(argv), scope_(scope) {
}

Object* LambdaCell::Eval(Scope* scope) {
    auto symb = As<LambdaSymbol>(first_);
    auto func = As<Function>(
        Heap::GetHeap().Allocate<LambdaFunction>(symb->GetArgc(), symb->GetVarc(), scope));
    auto args = func->CollectArgs(second_, scope);
    return func->Apply(args, scope);
}

Object* LambdaFunction::Apply(std::vector<Object*>& args, Scope* scope) {
    return Heap::GetHeap().Allocate<LambdaInvoker>(argv_, argc_, scope_, args);
}

LambdaInvoker::LambdaInvoker(int argc, int argv, Scope* scope, std::vector<Object*>& state)
    : FunctionEval(argc), argv_(argv), scope_(scope), state_(state) {
}

std::string LambdaInvoker::ToString() {
    std::vector<Object*> temp;
    auto res = LambdaInvoker::Apply(temp, scope_);
    return res->ToString();
}

Scope::Scope(Scope* scope) : prev_scope_(scope) {
}

Object* LambdaInvoker::Apply(std::vector<Object*>& args, Scope* scope) {
    if (args.size() != argc_) {
        throw RuntimeError("lambda invalid args");
    }

    Scope* new_scope = As<Scope>(Heap::GetHeap().Allocate<Scope>(scope_));
    for (int i = 0; i < args.size(); i++) {
        new_scope->AddForce(As<Symbol>(As<Cell>(state_[i])->GetFirst())->GetName(), args[i]);
    }

    Object* result = nullptr;
    for (int i = args.size(); i < state_.size(); i++) {
        result = state_[i]->Eval(new_scope);
    }
    return result;
}

Object* LambdaInvoker::Eval(Scope* scope) {
    return Heap::GetHeap().Allocate<LambdaInvoker>(*this);
}

bool Scope::AddForce(const std::string& key, Object* obj) {
    map_[key] = obj;
    return true;
}

int LambdaInvoker::GetArgc() {
    return argc_;
}

void Object::Mark() {
    marked_ = true;
}

void Object::Unmark() {
    marked_ = false;
}

void Scope::Mark() {
    if (marked_) {
        return;
    }

    marked_ = true;
    for (auto [k, v] : map_) {
        v->Mark();
    }

    if (prev_scope_) {
        prev_scope_->Mark();
    }
}

Object* Scope::Eval(Scope* scope) {
    throw RuntimeError("Can't eval scope");
}

std::string Scope::ToString() {
    throw RuntimeError("Can't serialize scope");
}

void Cell::Mark() {
    if (marked_) {
        return;
    }

    marked_ = true;
    if (GetFirst() != nullptr) {
        GetFirst()->Mark();
    }
    if (GetSecond() != nullptr) {
        GetSecond()->Mark();
    }
}

void LambdaCell::Mark() {
    if (marked_) {
        return;
    }

    marked_ = true;
    if (first_ != nullptr) {
        first_->Mark();
    }
    if (second_ != nullptr) {
        second_->Mark();
    }
}

void LambdaFunction::Mark() {
    if (marked_) {
        return;
    }

    marked_ = true;
    if (scope_) {
        scope_->Mark();
    }
}

void List::Mark() {
    if (marked_) {
        return;
    }

    marked_ = true;
    for (auto obj : state_) {
        obj->Mark();
    }
}

void LambdaInvoker::Mark() {
    if (marked_) {
        return;
    }

    marked_ = true;
    for (auto obj : state_) {
        obj->Mark();
    }
    if (scope_) {
        scope_->Mark();
    }
}
