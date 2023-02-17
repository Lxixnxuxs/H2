//
// Created by ray on 17.02.23.
//

#ifndef H2_ASTCLASSNODE_HPP
#define H2_ASTCLASSNODE_HPP

#include "ASTNode.hpp"
#include <memory>

struct LocalVariableManager;
struct ASTFunctionNode;


struct ASTClassNode : ASTNode {
    std::vector<std::shared_ptr<ASTFunctionNode>> methods;
    std::string name;
    int offset;
    //std::unique_ptr<LocalVariableManager> object_variables;
    std::vector<std::string> runtime_parameters;

    ASTClassNode(std::string name, int offset, std::vector<std::string> runtime_parameters);

    std::string compile() override;

    std::string to_code() override;


};

#endif //H2_ASTCLASSNODE_HPP
