//
// Created by Linus on 18.02.2023.
//

#ifndef H2_ASTVARIABLENODE_HPP
#define H2_ASTVARIABLENODE_HPP

#include "ASTStatementNode.hpp"

#include "../Tokenstream.hpp"

#include "memory"

struct LocalVariableManager;
struct GlobalVariableManager;

struct ASTVariableNode : ASTStatementNode {
    std::string name;

    std::string reg;

    bool is_root;

    std::shared_ptr<ASTVariableNode> child;
    std::shared_ptr<LocalVariableManager> local_vars;
    std::shared_ptr<GlobalVariableManager> global_vars;

    ASTVariableNode(std::string name, std::shared_ptr<ASTVariableNode> child,
                    bool is_root, std::shared_ptr<LocalVariableManager> local_vars, std::shared_ptr<GlobalVariableManager> global_vars,
                    std::string reg): name(name), child(child), is_root(is_root), local_vars(local_vars),
                                      global_vars(global_vars), reg(reg) {};

    std::string compile(std::string);

    void set_block_level(int n){};

    std::string compile(){};

    VirtualMathTerm get_complexity(){};

    std::string to_code(){};

    std::string get_class(){};

};

#endif //H2_ASTVARIABLENODE_HPP
