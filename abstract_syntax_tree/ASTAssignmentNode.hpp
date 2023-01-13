#ifndef H2_ASTASSIGNMENTNODE_HPP
#define H2_ASTASSIGNMENTNODE_HPP

#include "ASTStatementNode.hpp"
#include "ASTCalculationNode.hpp"

struct ASTAssignmentNode : ASTStatementNode {

    size_t offset;
    std::string var_name;

    ASTComputationNode* right;

    ASTAssignmentNode(size_t offset, ASTComputationNode* right, std::string var_name):
             offset(offset), right(right), var_name(var_name) {}

    std::string compile() override{
        std::string code = right->compile();
        code += "mov " + right->reg + ", " + std::to_string(offset) + "(%rsp)\n";

        return code;
    }

    Term* calculate_complexity() override {
        auto a = right->calculate_complexity();
        complexity = a;
        return a;
    }

    std::string to_code() override {
        return var_name + " = " + right->to_code() + ";\n";
    }
};

#endif //H2_ASTASSIGNMENTNODE_HPP