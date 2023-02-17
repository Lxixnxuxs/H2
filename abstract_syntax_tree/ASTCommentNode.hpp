//
// Created by ray on 16.02.23.
//

#ifndef H2_ASTCOMMENTNODE_HPP
#define H2_ASTCOMMENTNODE_HPP

#include "ASTStatementNode.hpp"

struct ASTCommentNode : ASTStatementNode {
    std::string text;

    ASTCommentNode(std::string text);

    std::string get_class();

    std::string to_code();

    std::string compile();

    VirtualMathTerm get_complexity();

    //void set_block_level(int n);
};

#endif //H2_ASTCOMMENTNODE_HPP
