#ifndef H2_ASTWHILELOOPNODE_HPP
#define H2_ASTWHILELOOPNODE_HPP

#include "ASTControlFlowNode.hpp"
#include "ASTComparisonNode.hpp"

struct ASTWhileLoopNode : ASTControlFlowNode {
    ASTComparisonNode* condition;
    std::vector<ASTStatementNode*> block;
    int label_id;

    ASTWhileLoopNode(ASTComparisonNode* condition, std::vector<ASTStatementNode*> &block,int label_id): condition(condition),
    block(block), label_id(label_id) {};


    std::string compile() override {
        std::string code = "jmp L_WHILE_COND_"+std::to_string(label_id) +
                "\nL_WHILE_BODY_"+std::to_string(label_id)+":\n";

        for (auto& node : block) {
            code += node->compile();
        }

        code+= "L_WHILE_COND_"+std::to_string(label_id)+":\n";

        code += condition->compile();

        code += comp_to_jump[condition->comp_type]+" L_WHILE_BODY_" + std::to_string(label_id) + "\n";

        return code;
    }

    Term* calculate_complexity() override {
        auto* a = new Term(ADDITION);

        a->children.push_back(condition->calculate_complexity());

        // TODO I need a multiply logic here. Not jet correct
        for (auto e : block) {
            a->children.push_back(e->calculate_complexity());
        }
        auto* b = new Term(MULTIPLICATION);
        b->children.push_back(a);
        b->children.push_back(new Term(VARIABLE,"iter"));
        complexity = b;
        return complexity;
    }
};

#endif //H2_ASTWHILELOOPNODE_HPP
