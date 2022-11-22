#include <iostream>
#include "parser.hpp"
#include "Expression.hpp"


// printing vectors
template <typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> vec){
    os << "[ ";
    for (T e : vec){
        os << e << "\n";
    }
    os << "]";

    return os;
}



int main() {
    std::cout << "Hello, World!" << std::endl;

    string file = "/home/ray/CLionProjects/H2/resources/program.txt";//"./resources/program.txt";//"a1.txt";//"/home/ray/CLionProjects/H2/resources/program.txt"; //"
    vector<string> content = read_file(file);
    vector<vector<string>> funcs = split_functions(content);
    cout << content<<endl;
    parse_func(funcs[0]);
    return 0;

}
