//
// Created by ray on 08.12.22.
//

#ifndef H2_ASTCALLNODE_HPP
#define H2_ASTCALLNODE_HPP

#include "ASTCalculationNode.hpp"
struct ASTFunctionNode;


struct ASTCallNode : ASTCalculationNode{
    ASTFunctionNode* target;
    std::string target_name;
    std::vector<ASTCalculationNode*> arguments;
    int h;

    ASTCallNode(ASTCalculationNode *left, ASTCalculationNode *right, ComputationOp compType, std::string reg, int value,
                size_t offset, ASTFunctionNode* target_, std::vector<ASTCalculationNode*> arguments, int h);
            // call node is always taking register eax

    ASTCallNode()=default;

    std::string compile() override;

    VirtualMathTerm calculate_complexity() override;

    std::string to_code() override;

    std::string get_class() override;

    VirtualMathTerm as_math_term() override;
};

#endif //H2_ASTCALLNODE_HPP
