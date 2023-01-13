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
        code += "mov " + calc->reg + ", %eax\n";
        code += "jmp END_" + f_name + '\n';

        return code;
    }

    Term* calculate_complexity() override {
        auto a = calc->calculate_complexity();
        complexity = a;
        return a;
    }

    std::string to_code() override {
        return "return " + calc->to_code();
    }
};

#endif //H2_ASTRETURNNODE_HPP
