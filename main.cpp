#include "lisp.h"

#include <iostream>

int main() {
    Interpreter interpreter;

    while (true) {
        std::string line;
        std::getline(std::cin, line);
        try {
            std::cout << interpreter.Run(line) << std::endl;
        } catch (std::exception& ex) {
            std::cout << ex.what() << std::endl;
        }
    }
}
