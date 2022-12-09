//
// Created by ray on 08.12.22.
//

#include "ASTStatementNode.hpp"
#include "ASTFunctionNode.hpp"
#include "ASTCalculationNode.hpp"
#include "../global_information.hpp"

#ifndef H2_ASTCALLNODE_HPP
#define H2_ASTCALLNODE_HPP

struct ASTCallNode : ASTCalculationNode{

    std::string target;
    std::vector<ASTCalculationNode*> arguments;
    int h;

    ASTCallNode(ASTCalculationNode *left, ASTCalculationNode *right, ComputationOp compType, std::string reg, int value,
                size_t offset, std::string target_, std::vector<ASTCalculationNode*> arguments, int h) : ASTCalculationNode(
            left, right, compType, regs[h], value, offset), target(target_), arguments(arguments), h(h) {}
            // call node is always taking register eax

    std::string compile() override{
        std::string code;
        for (int i = 0; i<arguments.size(); i++) {
            code += arguments[i]->compile();
            code += "mov "+regs[i+h]+", "+argument_regs[i]+"\n";   // put subcalculation-result in corresponding argument place
        }

        code += "call "+target+"\n";
        code += "mov %eax, "+regs[h]+"\n";
        return code;
    }
};

#endif //H2_ASTCALLNODE_HPP
