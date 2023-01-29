//
// Created by ray on 20.01.23.
//
#include "ASTRootNode.hpp"
#include "ASTFunctionNode.hpp"


ASTRootNode::ASTRootNode(std::vector<ASTFunctionNode*>& funcs): funcs(funcs) {}

    std::string ASTRootNode::compile() {
        std::string code = "jmp main\n\n"; // always starting with main function

        for (auto f : funcs) {
            code += f->compile();
        }

        return code;
    }

    VirtualMathTerm ASTRootNode::calculate_complexity() {

        // sub calls
        for (auto e : funcs) {
            e->calculate_complexity();
        }

        // only functions eg have meaningful complexity. Not the root node itself
        complexity = VirtualMathTerm("-"); // placeholder
        return complexity;
    }

    std::string ASTRootNode::to_code() {
        std::string res;
        for (auto e : funcs) {
            res += e->to_code() + "\n\n";
        }
        return res;
    }

    void ASTRootNode::set_block_level(int n) {
        block_level = 0;
        for (auto f: funcs) f->set_block_level(0);
    }

    std::string ASTRootNode::get_class() { return "Root";}


