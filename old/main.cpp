#include <iostream>
#include <cstdlib>
#include "parser.hpp"
#include "Expression.hpp"
#include "ParserMain.hpp"

int main() {
    std::string filename = "/home/linus/transfer/wsl_transfer/H2/resources/program.txt";
    ParserMain p(filename);
/*
    //std::cout << p.compile() << endl;
    std::ifstream f0("/home/linus/transfer/wsl_transfer/H2/resources/header.txt");
    std::ifstream f1("/home/linus/transfer/wsl_transfer/H2/resources/end_linus.txt");
    std::stringstream buffer0;
    std::stringstream buffer1;

    buffer0 << f0.rdbuf();
    buffer1 << f1.rdbuf();

    std::ofstream out("as.s");
    out << buffer0.str() << p.compile() << buffer1.str();
    out.close();

 */

    std::cout << p.compile() << endl;

    return 0;
}