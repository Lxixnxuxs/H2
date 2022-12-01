
#ifndef H2_ASTCOMPUTATIONNODE_HPP
#define H2_ASTCOMPUTATIONNODE_HPP

#include "ASTStatementNode.hpp"

enum ComputationOp {LIT, VAR, ADD, SUB, MUL};

struct ASTComputationNode : ASTStatementNode {
    ASTComputationNode* left;
    ASTComputationNode* right;

    ComputationOp comp_type;

    std::string reg;
    bool own_reg = true;

    // Optional variables for cases (LIT, VAR)
    int value;
    size_t offset;

    ASTComputationNode(ASTComputationNode* left, ASTComputationNode* right, ComputationOp comp_type, std::string reg = "",
                       int value = 0, size_t offset = 0):
    left(left), right(right), comp_type(comp_type), reg(reg), value(value), offset(offset){
        if (reg.empty() && left != nullptr) {reg = left->reg; own_reg = false;}
    }

    virtual std::string compile() {
        if (comp_type == VAR) {
            return "movq -" + std::to_string(offset) + "(%rsp)" + ", " + reg + "\n";
        } else if (comp_type == LIT) {
            return "movq $" + std::to_string(value) + "," + reg + "\n";
        } else {
            // Recursive code generation
            if (left->comp_type == LIT && right->comp_type == LIT) {
                return "movq $" + optimize_literal_computation() + ", " + reg + "\n";
            } else {
                std::string code = left->compile() + right->compile();

                code += computation_op_to_string() + " " + right->reg + "," + left->reg +
                (own_reg ? "\nmovq " + left->reg + "," + reg + "\n" : "\n");

                return code;
            }
        }
    }

    std::string computation_op_to_string() {
        switch (comp_type) {
            case ADD: return "addq";
            case SUB: return "subq";
            case MUL: return "mulq";
        }
        return "";
    }

    std::string optimize_literal_computation() {
        switch(comp_type) {
            case ADD: return std::to_string(left->value + right->value);
            case SUB: return std::to_string(left->value - right->value);
            case MUL: return std::to_string(left->value * right->value);
        }
        return "";
    }
};

#endif //H2_ASTCOMPUTATIONNODE_HPP
