#include <fstream>
#include <iostream>
#include "CodeParser.hpp"
#include "lexer.hpp"

int main() {
    std::string path = "/home/linus/transfer/wsl_transfer/H2/resources/program.txt";
    std::ifstream file(path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    Tokenstream t = lexer(content);

    CodeParser parser(t);
    ASTRootNode* root = parser.parse(t);
    std::string compilation = root->compile();

    std::cout << compilation;

    return 0;
}