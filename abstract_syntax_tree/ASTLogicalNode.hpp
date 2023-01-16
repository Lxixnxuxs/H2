#ifndef H2_ASTLOGICALNODE_HPP
#define H2_ASTLOGICALNODE_HPP

#include "ASTComputationNode.hpp"

enum LogicalOp {AND, OR, NOT, GT, LT, GT_EQ, LT_EQ, EQ};

namespace ASTLogicalNodeHelper {

    bool is_binary_logic_op(LogicalOp op) {
        return op == AND || op == OR || op == NOT;
    }

    bool is_binary_cmp_op(LogicalOp op) {
        return !is_binary_logic_op(op);
    }
}

struct ASTLogicalNode : ASTComputationNode {
    ASTComputationNode* left;
    ASTComputationNode* right;

    LogicalOp op_type;

    bool own_reg = true;

    ASTLogicalNode(ASTComputationNode* left, ASTComputationNode* right, LogicalOp op_type, std::string reg=""):
        left(left), right(right), op_type(op_type) {
        this->reg = reg;
        if (reg.empty() && left != nullptr) {reg = ""; own_reg = false;}
    }

    std::string compile() override {
        std::string code = "";
        if (right == nullptr) {
            // Unary Logical expression
        } else {
            // Binary logical expression
            code += left->compile();
            code += right->compile();


        }
        return code;
    }

    std::string logical_op_to_string() {
        switch (op_type) {
            case AND: return "andq";
            case OR: return "orq";
            case NOT: return "notq";
            default: return "cmp";
        }
    }

    std::string get_class() override { return "Logical";}
};

#endif //H2_ASTLOGICALNODE_HPP
