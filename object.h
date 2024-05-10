#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

class Heap;
class Scope;

class Object {
    friend class Heap;

public:
    virtual ~Object() = default;
    virtual Object* Eval(Scope* scope) = 0;
    virtual std::string ToString() = 0;

    virtual void Mark();
    virtual void Unmark();

protected:
    bool marked_ = false;
};

class Scope : public Object {
public:
    Scope(Scope* scope = nullptr);
    Object* Get(const std::string& key);
    bool Add(const std::string& key, Object* obj, bool force_add);
    bool AddForce(const std::string& key, Object* obj);

    virtual Object* Eval(Scope* scope) override;
    virtual std::string ToString() override;
    virtual void Mark() override;

private:
    Scope* prev_scope_;
    std::unordered_map<std::string, Object*> map_;
};

class Number : public Object {
public:
    Number(int64_t value);
    int64_t GetValue() const;
    virtual Object* Eval(Scope* scope) override;
    virtual std::string ToString() override;

private:
    int64_t value_;
};

class FakeNumber : public Number {
public:
    using Number::Number;
};

class Symbol : public Object {
public:
    Symbol(const std::string& str);
    const std::string& GetName() const;
    void AddArgc(int argc);
    void SetArgc(int argc);
    int GetArgc() const;

    virtual Object* Eval(Scope* scope) override;
    virtual std::string ToString() override;

protected:
    std::string name_;
    int argc_ = 0;
};

class LambdaSymbol : public Symbol {
public:
    using Symbol::Symbol;
    void SetVarc(int varc);
    int GetVarc() const;

private:
    int varc_ = 0;
};

class Boolean : public Object {
public:
    Boolean(bool value);
    bool GetValue() const;

    virtual Object* Eval(Scope* scope) override;
    virtual std::string ToString() override;

private:
    bool value_;
};

class Cell : public Object {
public:
    Cell(Object* first);

    Object* GetFirst() const;
    Object* GetSecond() const;

    void SetFirst(Object* ptr);
    void SetSecond(Object* ptr);

    virtual Object* Eval(Scope* scope) override;
    virtual std::string ToString() override;

    virtual void Mark() override;

private:
    Object* first_;
    Object* second_;
};

class LambdaCell : public Object {
public:
    LambdaCell(Object* first);

    void SetFirst(Object* ptr);
    void SetSecond(Object* ptr);

    virtual Object* Eval(Scope* scope) override;
    virtual std::string ToString() override;
    virtual void Mark() override;

private:
    Object* first_;
    Object* second_;
};

class Function : public Object {
public:
    Function(int argc);
    virtual Object* Eval(Scope* scope) override;
    virtual std::string ToString() override;

    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) = 0;
    virtual std::vector<Object*> CollectArgs(Object* root, Scope* scope) = 0;

protected:
    int argc_;
};

class FunctionEval : public Function {
public:
    using Function::Function;
    virtual std::vector<Object*> CollectArgs(Object* root, Scope* scope) override;
};

class FunctionNoEval : public Function {
public:
    using Function::Function;
    virtual std::vector<Object*> CollectArgs(Object* root, Scope* scope) override;
};

class LambdaFunction : public FunctionNoEval {
public:
    LambdaFunction(int argc, int argv, Scope* scope);
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
    virtual void Mark() override;

private:
    int argv_;
    Scope* scope_;
};

class LambdaInvoker : public FunctionEval {
public:
    LambdaInvoker(int argc, int argv, Scope* scope, std::vector<Object*>& state);
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
    virtual Object* Eval(Scope* scope) override;
    virtual std::string ToString() override;
    virtual void Mark() override;

    int GetArgc();

private:
    int argv_;
    Scope* scope_;
    std::vector<Object*> state_;
};

class DefineVar : public FunctionNoEval {
public:
    using FunctionNoEval::FunctionNoEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class SetVar : public FunctionNoEval {
public:
    using FunctionNoEval::FunctionNoEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class And : public FunctionNoEval {
public:
    using FunctionNoEval::FunctionNoEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class Or : public FunctionNoEval {
public:
    using FunctionNoEval::FunctionNoEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class If : public FunctionNoEval {
public:
    using FunctionNoEval::FunctionNoEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class QuoteFunction : public FunctionNoEval {
public:
    using FunctionNoEval::FunctionNoEval;
    virtual std::vector<Object*> CollectArgs(Object* root, Scope* scope) override;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class List : public Object {
public:
    List(const std::vector<Object*>& state);
    Object* Get(size_t ind);
    void Set(size_t ind, Object* obj);
    int Size();
    bool IsMalformed();

    virtual Object* Eval(Scope* scope) override;
    virtual std::string ToString() override;
    virtual void Mark() override;

private:
    bool IsObject(size_t i);
    std::vector<Object*> state_;
};

class IsNumber : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class IsPair : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class IsList : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class IsSymbol : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class MakeList : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class ListTail : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class ListRef : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class IsNull : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class IsBoolean : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class Not : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class MakePair : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class Head : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class SetHead : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class Tail : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class SetTail : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class Equal : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class Less : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class Greater : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class LessEqual : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class GreaterEqual : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class Sum : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class Difference : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class Product : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class Division : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class Max : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class Min : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

class Abs : public FunctionEval {
public:
    using FunctionEval::FunctionEval;
    virtual Object* Apply(std::vector<Object*>& args, Scope* scope) override;
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
T* As(Object* obj) {
    return dynamic_cast<T*>(obj);
}

template <class T>
bool Is(Object* obj) {
    return dynamic_cast<T*>(obj) != nullptr;
}
