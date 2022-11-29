#include <iostream>
#include <cstdlib>
#include "parser.hpp"
#include "Expression.hpp"
#include "ParserMain.hpp"

int main() {
    std::string filename = "/home/ray/CLionProjects/H2/resources/program.txt";
    ParserMain p(filename);

    //std::cout << p.compile() << endl;
    std::ofstream out("as.s");
    out << p.compile() << ".LFB0:\n"
                          "\t.cfi_startproc\n"
                          "\tendbr64\n"
                          "\tpushq\t%rbp\n"
                          "\t.cfi_def_cfa_offset 16\n"
                          "\t.cfi_offset 6, -16\n"
                          "\tmovq\t%rsp, %rbp\n"
                          "\t.cfi_def_cfa_register 6\n"
                          "\tmovl\t%edi, -4(%rbp)\n"
                          "\tmovq\t%rsi, -16(%rbp)\n"
                          "\tmovl\t$0, %eax\n"
                          "\tpopq\t%rbp\n"
                          "\t.cfi_def_cfa 7, 8\n"
                          "\tret\n"
                          "\t.cfi_endproc\n"
                          ".LFE0:\n"
                          "\t.size\tmain, .-main\n"
                          "\t.ident\t\"GCC: (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0\"\n"
                          "\t.section\t.note.GNU-stack,\"\",@progbits\n"
                          "\t.section\t.note.gnu.property,\"a\"\n"
                          "\t.align 8\n"
                          "\t.long\t 1f - 0f\n"
                          "\t.long\t 4f - 1f\n"
                          "\t.long\t 5\n"
                          "0:\n"
                          "\t.string\t \"GNU\"\n"
                          "1:\n"
                          "\t.align 8\n"
                          "\t.long\t 0xc0000002\n"
                          "\t.long\t 3f - 2f\n"
                          "2:\n"
                          "\t.long\t 0x3\n"
                          "3:\n"
                          "\t.align 8\n"
                          "4:\n";
    out.close();

    return 0;
}