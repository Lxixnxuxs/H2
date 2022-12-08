//
// Created by ray on 08.12.22.
//

#include "ASTStatementNode.hpp"
#include "ASTFunctionNode.hpp"
#include "ASTCalculationNode.hpp"
#include "../global_information.hpp"

#ifndef H2_ASTCALLNODE_HPP
#define H2_ASTCALLNODE_HPP

struct ASTCallNode : ASTStatementNode{

    ASTFunctionNode target;
    std::vector<ASTCalculationNode> arguments;

    ASTCallNode(ASTFunctionNode target_, std::vector<ASTCalculationNode> arguments): target(target_), arguments(arguments){}

    std::string compile() override{
        std::string code;
        for (int i = 0; i<arguments.size(); i++) {
            code += arguments[i].compile();
            code += "movq "+regs[0]+" "+argument_regs[i];   //
        }
    }
};

#endif //H2_ASTCALLNODE_HPP
