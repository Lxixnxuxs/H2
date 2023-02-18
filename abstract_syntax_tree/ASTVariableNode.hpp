//
// Created by Linus on 18.02.2023.
//

#ifndef H2_ASTVARIABLENODE_HPP
#define H2_ASTVARIABLENODE_HPP

#include "ASTStatementNode.hpp"

#include "memory"

struct LocalVariableManager;
struct GlobalVariableManager;

struct ASTVariableNode : ASTStatementNode {
    std::string name;

    std::string reg;

    bool is_root;

    std::shared_ptr<ASTVariableNode> child;
    LocalVariableManager* local_vars;
    GlobalVariableManager* global_vars;


    std::string compile(std::string);

};

#endif //H2_ASTVARIABLENODE_HPP
