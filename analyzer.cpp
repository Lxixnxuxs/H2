//
// Created by ray on 12.01.23.
//
#include <fstream>
#include <iostream>
#include "CodeParser.hpp"
#include "lexer.hpp"
#include "abstract_syntax_tree/ASTRootNode.hpp"

int main() {
    std::string path = "./resources/raw.txt";
    std::ifstream file(path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    Tokenstream t = lexer(content);

    CodeParser parser;
    ASTRootNode* root = parser.parse(t);
    root->get_complexity();
    root->set_block_level(0);


    auto new_code = root->to_code();
    //std::cout << new_code;

    std::ofstream ofile("./resources/raw.txt");
    ofile << new_code;
    ofile.close();
    return 0;

}

