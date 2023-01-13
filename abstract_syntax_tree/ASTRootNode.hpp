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

    Term * calculate_complexity() override{

        // sub calls
        for (auto e : funcs) {
            e->calculate_complexity();
        }

        // only functions eg have meaningful complexity. Not the root node itself
        complexity = new Term(VARIABLE,"-"); // placeholder
        return complexity;
    }

    std::string to_code() override {
        std::string res;
        for (auto e : funcs) {
            res += e->to_code() + "\n\n";
        }
        return res;
    }
};

#endif //H2_ASTROOTNODE_HPP
