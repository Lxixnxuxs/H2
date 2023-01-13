//
// Created by ray on 12.01.23.
//
#include <fstream>
#include <iostream>
#include "CodeParser.hpp"
#include "lexer.hpp"

int main() {
    std::string path = "/home/ray/CLionProjects/H2/resources/program.txt";
    std::ifstream file(path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    Tokenstream t = lexer(content);

    CodeParser parser;
    ASTRootNode* root = parser.parse(t);
    root->calculate_complexity();
    for (auto& e : root->funcs) {
        cout << e->f_name << " : " <<  e->complexity->as_string() << endl;
    }
    /*std::cout << compilation;

    std::ofstream ofile("/home/ray/CLionProjects/H2/resources/raw.txt");
    ofile << ".globl main\n";
    ofile << compilation;
    ofile.close();*/
    return 0;
}