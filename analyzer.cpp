//
// Created by ray on 12.01.23.
//
#include <fstream>
#include <iostream>
#include "CodeParser.hpp"
#include "lexer.hpp"

int main() {
    /*
    list<string> list = {"/%","123","=","%/","return"};

    auto ts = Tokenstream(&list);
    auto t2 = ts.read_inside_brackets();

    cout << t2 << endl;
    cout << ts << endl;
    */
    std::string path = "/home/ray/CLionProjects/H2/resources/raw.txt";
    std::ifstream file(path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    Tokenstream t = lexer(content);

    CodeParser parser;
    ASTRootNode* root = parser.parse(t);
    root->calculate_complexity();
    for (auto& e : root->funcs) {
        cout << e->f_name << " : " <<  e->complexity->as_string() << endl;
    }

    auto new_code = root->to_code();
    std::cout << new_code;

    std::ofstream ofile("/home/ray/CLionProjects/H2/resources/raw.txt");
    ofile << new_code;
    ofile.close();
    return 0;

}