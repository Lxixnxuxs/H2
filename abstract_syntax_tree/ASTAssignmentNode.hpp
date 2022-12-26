#ifndef H2_ASTASSIGNMENTNODE_HPP
#define H2_ASTASSIGNMENTNODE_HPP

#include "ASTStatementNode.hpp"
#include "ASTCalculationNode.hpp"

struct ASTAssignmentNode : ASTStatementNode {

    size_t offset;

    ASTComputationNode* right;

    ASTAssignmentNode(size_t offset, ASTComputationNode* right):
             offset(offset), right(right) {}

    std::string compile() {
        std::string code = right->compile();
        code += "mov " + right->reg + ", " + std::to_string(offset) + "(%rsp)\n";

        return code;
    }
};

#endif //H2_ASTASSIGNMENTNODE_HPP