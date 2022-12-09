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

    ASTCallNode(ASTCalculationNode *left, ASTCalculationNode *right, ComputationOp compType, std::string reg, int value,
                size_t offset, std::string target_, std::vector<ASTCalculationNode*> arguments) : ASTCalculationNode(
            left, right, compType, "%eax", value, offset), target(target_), arguments(arguments) {}
            // call node is always taking register eax
            // TODO what if multiple calls are processed like  f(x) * g(x)  ?

    std::string compile() override{
        std::string code;
        for (int i = 0; i<arguments.size(); i++) {
            code += arguments[i]->compile();
            code += "mov "+regs[0]+", "+argument_regs[i]+"\n";   // put result in corresponding argument place
        }

        code += "call "+target+"\n";    // TODO where on the stack goes the return adress?
        return code;
    }
};

#endif //H2_ASTCALLNODE_HPP
