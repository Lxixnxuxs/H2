
#include "ASTLogicalNode.hpp"



namespace ASTLogicalNodeHelper {

    bool is_binary_logic_op(LogicalOp op) {
        return op == AND || op == OR || op == NOT;
    }

    bool is_binary_cmp_op(LogicalOp op) {
        return !is_binary_logic_op(op);
    }
}


ASTLogicalNode::ASTLogicalNode(ASTComputationNode* left, ASTComputationNode* right, LogicalOp op_type, std::string reg):
            left(left), right(right), op_type(op_type) {
        this->reg = reg;
        if (reg.empty() && left != nullptr) {reg = ""; own_reg = false;}
    }

    std::string ASTLogicalNode::compile() {
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

    std::string ASTLogicalNode::logical_op_to_string() {
        switch (op_type) {
            case AND: return "andq";
            case OR: return "orq";
            case NOT: return "notq";
            default: return "cmp";
        }
    }

    std::string ASTLogicalNode::get_class() { return "Logical";}

