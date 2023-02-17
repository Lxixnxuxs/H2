//
// Created by ray on 17.02.23.
//

#include "ASTClassNode.hpp"
#include "ASTFunctionNode.hpp"

std::string ASTClassNode::compile() {
    std::string code = "; " + name + "\n";
    for (auto method : methods) {
        code += method->compile();
    }
    return code;
}