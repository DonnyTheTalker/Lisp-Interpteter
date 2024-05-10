#pragma once

#include <string>
#include <memory>

#include "object.h"

class Interpreter {
public:
    Interpreter();
    ~Interpreter();
    std::string Run(const std::string&);

private:
    void ClearUnused();

private:
    Scope* scope_;
};
