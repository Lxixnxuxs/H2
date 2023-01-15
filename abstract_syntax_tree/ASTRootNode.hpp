#ifndef H2_ASTROOTNODE_HPP
#define H2_ASTROOTNODE_HPP

#include "ASTNode.hpp"
#include <vector>

struct ASTRootNode : ASTNode {

    std::vector<ASTFunctionNode*> funcs;

    ASTRootNode(std::vector<ASTFunctionNode*>& funcs): funcs(funcs) {}

    std::string compile() {
        std::string code = "jmp main\n\n"; // always starting with main function

        for (ASTNode* f : funcs) {
            code += f->compile();
        }

        return code;
    }

    ComplexityTerm * calculate_complexity() override{

        // sub calls
        for (auto e : funcs) {
            e->calculate_complexity();
        }

        // only functions eg have meaningful complexity. Not the root node itself
        complexity = new ComplexityTerm(VARIABLE, "-"); // placeholder
        return complexity;
    }

    std::string to_code() override {
        std::string res;
        for (auto e : funcs) {
            res += e->to_code() + "\n\n";
        }
        return res;
    }

    void set_block_level(int n) {
        block_level = 0;
        for (auto f: funcs) f->set_block_level(0);
    }
};

#endif //H2_ASTROOTNODE_HPP
