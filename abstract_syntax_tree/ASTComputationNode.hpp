#ifndef H2_ASTCOMPUTATIONNODE_HPP
#define H2_ASTCOMPUTATIONNODE_HPP

#include "ASTStatementNode.hpp"

struct ASTComputationNode : ASTStatementNode {
    std::string reg;

    virtual VirtualMathTerm as_math_term() = 0;
};

#endif //H2_ASTCOMPUTATIONNODE_HPP
