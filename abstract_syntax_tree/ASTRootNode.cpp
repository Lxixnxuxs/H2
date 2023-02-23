//
// Created by ray on 20.01.23.
//
#include "ASTRootNode.hpp"
#include "ASTFunctionNode.hpp"



ASTRootNode::ASTRootNode(std::vector<std::shared_ptr<ASTNode>>& funcs_and_classes): funcs_and_classes(funcs_and_classes) {}

    std::string ASTRootNode::compile() {

        std::string code = "jmp main\n\n"; // always starting with main function

        for (auto f : funcs_and_classes) {
            code += f->compile();
        }

        return code;
    }

    VirtualMathTerm ASTRootNode::get_complexity() {

        // sub calls
        for (auto e : funcs_and_classes) {
            e->get_complexity();
        }

        // only functions eg have meaningful complexity. Not the root node itself
        complexity = VirtualMathTerm("-"); // placeholder
        return complexity;
    }

    std::string ASTRootNode::to_code() {
        std::string res;
        for (auto e : funcs_and_classes) {
            res += e->to_code() + "\n\n";
        }
        return res;
    }

    void ASTRootNode::set_block_level(int n) {
        block_level = 0;
        for (auto f: funcs_and_classes) f->set_block_level(0);
    }

    std::string ASTRootNode::get_class() { return "Root";}


