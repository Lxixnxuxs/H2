#ifndef H2_ASTLOGICALNODE_HPP
#define H2_ASTLOGICALNODE_HPP

#include "ASTComputationNode.hpp"

enum LogicalOp {AND, OR, NOT, GT, LT, GT_EQ, LT_EQ, EQ};

namespace ASTLogicalNodeHelper {

    bool is_binary_logic_op(LogicalOp op);

    bool is_binary_cmp_op(LogicalOp op);
}

struct ASTLogicalNode : ASTComputationNode {
    ASTComputationNode* left;
    ASTComputationNode* right;
    LogicalOp op_type;
    bool own_reg = true;

    ASTLogicalNode(ASTComputationNode* left, ASTComputationNode* right, LogicalOp op_type, std::string reg="");

    std::string compile() override;

    std::string logical_op_to_string();

    std::string get_class() override;
};

#endif //H2_ASTLOGICALNODE_HPP
