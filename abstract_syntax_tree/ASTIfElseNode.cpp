//
// Created by ray on 29.01.23.
//

#include "ASTLogicalNode.hpp"
#include "ASTStatementNode.hpp"
#include "ASTComparisonNode.hpp"

#include "ASTIfElseNode.hpp"



ASTIfElseNode::ASTIfElseNode(ASTComparisonNode* condition, std::vector<ASTStatementNode*> &if_block,
                  std::vector<ASTStatementNode*> &else_block, int label_id): condition(condition),
                                                                             if_block(if_block), else_block(else_block), label_id(label_id) {};

    std::string ASTIfElseNode::compile() {
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

    VirtualMathTerm ASTIfElseNode::get_complexity() {
        auto a = VirtualMathTerm(ADDITION);
        for (auto e : if_block) {
            a.children.push_back(e->get_complexity());
        }
        for (auto e : else_block) {
            a.children.push_back(e->get_complexity());
        }
        complexity = a;
        return a;

    }

    std::string ASTIfElseNode::to_code() {
        auto res = get_indentation(block_level) + "if (" + condition->to_code() + ") {\n";
        for (auto e : if_block) res += e->to_code();
        res += get_indentation(block_level) + "} else {\n";
        for (auto e : else_block) res += e->to_code();
        res += get_indentation(block_level) + "}\n";
        return res;
    }

    void ASTIfElseNode::set_block_level(int n) {
        block_level = n;
        for (auto f: if_block) f->set_block_level(n+1);
        for (auto f: else_block) f->set_block_level(n+1);

    }

    std::string ASTIfElseNode::get_class() { return "IfElse";}
