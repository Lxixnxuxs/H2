#ifndef H2_ASTIFELSENODE_HPP
#define H2_ASTIFELSENODE_HPP

#include "ASTControlFlowNode.hpp"
#include "ASTLogicalNode.hpp"
#include "ASTStatementNode.hpp"
#include "ASTCalculationNode.hpp"
#include "ASTComparisonNode.hpp"
#include <vector>

struct ASTIfElseNode : ASTControlFlowNode {
    ASTComparisonNode* condition;
    std::vector<ASTStatementNode*> if_block;
    std::vector<ASTStatementNode*> else_block;

    int label_id;

    ASTIfElseNode(ASTComparisonNode* condition, std::vector<ASTStatementNode*> &if_block,
                  std::vector<ASTStatementNode*> &else_block, int label_id): condition(condition),
                        if_block(if_block), else_block(else_block), label_id(label_id) {};

    std::string compile() override {
        std::string code = condition->compile();

        code += comp_to_jump[condition->comp_type] + " L_IF_" + std::to_string(label_id) + "\n";
        code += "jmp L_ELSE_"+std::to_string(label_id)+"\n";
        code += "L_IF_" + std::to_string(label_id) + ":\n";

        for (auto& node : if_block) {
            code += node->compile();
        }

        code += "jmp L_END_" + std::to_string(label_id) + "\n";
        code += "L_ELSE_" + std::to_string(label_id) + ":\n";

        for (auto& node : else_block) {
            code += node->compile();
        }

        code += "L_END_" + std::to_string(label_id) + ":\n";

        return code;
    }
};

#endif //H2_ASTIFELSENODE_HPP
