//
// Created by ray on 17.02.23.
//

#ifndef H2_ASTCLASSNODE_HPP
#define H2_ASTCLASSNODE_HPP

#include "ASTNode.hpp"
#include <memory>
#include "../LocalVariableManager.hpp"
//struct LocalVariableManager;
struct ASTFunctionNode;


struct ASTClassNode : ASTNode {
    std::vector<std::shared_ptr<ASTFunctionNode>> methods;
    std::string name;
    //int offset;
    LocalVariableManager object_variables;
    std::vector<std::string> complexity_parameters;
    int size_of;

    ASTClassNode(std::string name, std::vector<std::string> runtime_parameters, LocalVariableManager object_variables,
                 std::vector<std::shared_ptr<ASTFunctionNode>> methods);

    std::string compile() override;

    std::string to_code() override;

    std::string get_class() override;

    void set_block_level(int n) override;

};

#endif //H2_ASTCLASSNODE_HPP
