#include <iostream>
#include "parser.hpp"
#include "Expression.hpp"

int main() {
    string filename = "/home/linus/transfer/wsl_transfer/H2/resources/program.txt";
    Parser P(filename);

    for (ParsedFunction f : P.split_by_functions()) {
        std::cout << f.name << "|" << f.arguments << "|" << f.type << "|" << f.expression_block << "\n";
    }

    return 0;
}