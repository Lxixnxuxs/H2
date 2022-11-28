#include <iostream>
#include <cstdlib>
#include "parser.hpp"
#include "Expression.hpp"
#include "ParserMain.hpp"

int main() {
/*
    string filename = "/home/linus/transfer/wsl_transfer/H2/resources/program.txt";
    Parser P;
    std::ifstream fs(filename);

    P.read(fs);

    for (ParsedFunction f : P.split_by_functions()) {
        //std::cout << f.name << "|" << f.arguments << "|" << f.type << "|" << f.expression_block << "\n";

        for (auto e : f.extract_statements()) {
            std::cout << e << "\n";
        }
    }
    */

    string filename = "/home/linus/transfer/wsl_transfer/H2/resources/program.txt";
    ParserMain p(filename);

    std::cout << p.to_assembly() << endl;

    return 0;
}