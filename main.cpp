#include <iostream>
#include <cstdlib>
#include "parser.hpp"
#include "Expression.hpp"
#include "ParserMain.hpp"

int main() {
    std::string filename = "/home/linus/transfer/wsl_transfer/H2/resources/program.txt";
    ParserMain p(filename);

    //std::cout << p.compile() << endl;
    std::ofstream out("as.s");
    out << p.compile();
    out.close();

    return 0;
}