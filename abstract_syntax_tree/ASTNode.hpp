
#ifndef H2_ASTNODE_HPP
#define H2_ASTNODE_HPP

#include "../term.hpp"

enum ASTNodeType {};

struct ASTNode {
    Term* complexity;

    virtual std::string compile() = 0;

    virtual Term* calculate_complexity() = 0;
};

#endif //H2_ASTNODE_HPP
