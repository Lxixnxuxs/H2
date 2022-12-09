#include <string>
#include <iostream>
#include <fstream>
#include "Tokenstream.hpp"
#include "CodeParser.hpp"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: ./compiler [path_source_code] [path_assembly]" << std::endl;
        return 1;
    }

    // read source code
    std::string source_path = argv[1];
    std::string output_path = argv[2];

    std::ifstream file(source_path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    file.close();

    // lexer stage
    Tokenstream t = lexer(content);

    // parser stage
    CodeParser parser;
    ASTRootNode* root = parser.parse(t);

    // code generation stage
    std::string compilation = root->compile();

    // add x86_64 assembly header
    compilation = ".text\n.globl main\n" + compilation;

    // write assembly to output file

    std::ofstream output_file(output_path);

    output_file << compilation;

    output_file.close();

    return 0;
}