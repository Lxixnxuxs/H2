
#ifndef H2_ASTNODE_HPP
#define H2_ASTNODE_HPP

#include <string>

enum ASTNodeType {};

struct ASTNode {

    virtual std::string compile() = 0;
};

#endif //H2_ASTNODE_HPP
