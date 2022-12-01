#ifndef H2_ASTASSIGNMENTNODE_HPP
#define H2_ASTASSIGNMENTNODE_HPP

#include "ASTStatementNode.hpp"
#include "ASTComputationNode.hpp"

struct ASTAssignmentNode : ASTStatementNode {

    std::string var_name;
    size_t offset;

    ASTComputationNode* right;

    ASTAssignmentNode(std::string var_name, size_t offset, ASTComputationNode* right):
            var_name(var_name), offset(offset), right(right) {}

    std::string compile() {
        std::string code = right->compile();
        code += "movq " + right->reg + ", -" + std::to_string(offset) + "(%rsp)";

        return code;
    }
};

#endif //H2_ASTASSIGNMENTNODE_HPP