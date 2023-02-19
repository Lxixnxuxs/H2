//
// Created by ray on 08.12.22.
//

#ifndef H2_ASTCALLNODE_HPP
#define H2_ASTCALLNODE_HPP

#include "ASTCalculationNode.hpp"
struct ASTFunctionNode;


struct ASTCallNode : ASTCalculationNode{
    std::shared_ptr<ASTFunctionNode> target;
    std::string target_name;
    std::vector<std::shared_ptr<ASTCalculationNode>> arguments;
    int h;

    ASTCallNode(std::shared_ptr<ASTCalculationNode> left, std::shared_ptr<ASTCalculationNode> right, ComputationOp compType, std::string reg, int value,
                size_t offset, std::shared_ptr<ASTFunctionNode> target_, std::vector<std::shared_ptr<ASTCalculationNode>> arguments, int h);
            // call node is always taking register eax

    ASTCallNode()=default;

    std::string compile() override;

    VirtualMathTerm get_complexity() override;

    std::string to_code() override;

    std::string get_class() override;

    VirtualMathTerm as_math_term() override;

    std::pair<std::string, bool> get_return_type();

};

#endif //H2_ASTCALLNODE_HPP
