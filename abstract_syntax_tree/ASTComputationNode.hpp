#ifndef H2_ASTCOMPUTATIONNODE_HPP
#define H2_ASTCOMPUTATIONNODE_HPP

#include "ASTStatementNode.hpp"

struct ASTComputationNode : ASTStatementNode {
    std::string reg;

};

#endif //H2_ASTCOMPUTATIONNODE_HPP
