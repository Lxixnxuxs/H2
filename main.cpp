#include <fstream>
#include <iostream>
#include "CodeParser.hpp"
#include "FileEditor.hpp"
#include "abstract_syntax_tree/ASTRootNode.hpp"


int main() {
    std::string path = "./resources/raw.txt";
    FileEditor fe = path;

    CodeParser parser;
    std::shared_ptr<ASTRootNode> root = parser.parse({fe});
    std::string compilation = root->compile();

    // add some printing logic
    compilation += "putChar:\n"
                   "sub $8, %rsp\n"
                   "mov %rdi, %r12\n"
                   "mov %r12, (%rsp)        \n"
                   "mov     $1, %rax    \n"
                   "mov     $1, %edi      \n"
                   "mov     %rsp , %rsi  \n"
                   "mov     $1, %rdx      \n"
                   "syscall            \n"
                   "add     $8, %rsp\n"
                   "\n"
                   "ret\n"
                   "\n"
                   "";

    std::cout << compilation;

    std::ofstream ofile("./as.s");
    ofile << ".globl main\n";
    ofile << compilation;
    ofile.close();

    fe.apply_changes();
    return 0;
}