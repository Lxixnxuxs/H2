#ifndef H2_ASTWHILELOOPNODE_HPP
#define H2_ASTWHILELOOPNODE_HPP

#include "ASTControlFlowNode.hpp"

struct ASTWhileLoopNode : ASTControlFlowNode {
    ASTCalculationNode* condition;
    std::vector<ASTStatementNode*> block;
    int label_id;

    ASTWhileLoopNode(ASTCalculationNode* condition, std::vector<ASTStatementNode*> &block,int label_id): condition(condition),
    block(block), label_id(label_id) {};


    std::string compile() override {
        std::string code = "jmp L_WHILE_COND_"+std::to_string(label_id) +
                "\nL_WHILE_BODY_"+std::to_string(label_id)+":\n";

        for (auto& node : block) {
            code += node->compile();
        }

        code+= "L_WHILE_COND_"+std::to_string(label_id)+":\n";

        code += condition->compile();

        code += "jz L_WHILE_BODY_" + std::to_string(label_id) + "\n";

        return code;
    }
};

#endif //H2_ASTWHILELOOPNODE_HPP
