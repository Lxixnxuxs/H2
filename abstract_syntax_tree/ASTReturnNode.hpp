#ifndef H2_ASTRETURNNODE_HPP
#define H2_ASTRETURNNODE_HPP

#include "ASTStatementNode.hpp"
#include "ASTCalculationNode.hpp"

struct ASTReturnNode : ASTStatementNode {

    ASTCalculationNode* calc;
    std::string f_name;

    ASTReturnNode(ASTCalculationNode* calc, std::string f_name): calc(calc), f_name(f_name) {}

    std::string compile() {
        std::string code = calc->compile();
        code += "movq " + calc->reg + ", %rax\n";
        code += "jmp END_" + f_name + '\n';

        return code;
    }
};

#endif //H2_ASTRETURNNODE_HPP
