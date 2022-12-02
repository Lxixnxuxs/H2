#ifndef H2_ASTROOTNODE_HPP
#define H2_ASTROOTNODE_HPP

#include "ASTNode.hpp"
#include <vector>

struct ASTRootNode : ASTNode {

    std::vector<ASTFunctionNode*> funcs;

    ASTRootNode(std::vector<ASTFunctionNode*>& funcs): funcs(funcs) {}

    std::string compile() {
        std::string code = "";

        for (ASTNode* f : funcs) {
            code += f->compile();
        }

        return code;
    }
};

#endif //H2_ASTROOTNODE_HPP
