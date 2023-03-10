//
// Created by Linus on 18.02.2023.
//

#ifndef H2_ASTVARIABLENODE_HPP
#define H2_ASTVARIABLENODE_HPP

#include "ASTCalculationNode.hpp"

#include "../Tokenstream.hpp"

#include "memory"

struct LocalVariableManager;
struct GlobalVariableManager;

struct ASTVariableNode : ASTCalculationNode {
    std::string name;

    bool is_root;
    bool return_primitive_by_value;

    std::shared_ptr<ASTVariableNode> child;
    std::shared_ptr<LocalVariableManager> local_vars;
    std::shared_ptr<GlobalVariableManager> global_vars;

    ASTVariableNode(std::string name, std::shared_ptr<ASTVariableNode> child,
                    bool is_root, std::shared_ptr<LocalVariableManager> local_vars, std::shared_ptr<GlobalVariableManager> global_vars,
                    std::string reg, bool return_primitive_by_value = false);

    std::string compile_helper(std::string);

    void set_block_level(int n){};

    std::string compile() override;

    VirtualMathTerm get_complexity(){};

    std::string to_code(){};

    std::string get_class(){return "ASTVariableNode";};

    std::string get_resulting_type(std::string last_class_name = "");

private:
    std::string get_type(std::string last_class_name = "");

};

void follow_variable_reference_if_applying(std::shared_ptr<ASTComputationNode> node, std::string& code);

#endif //H2_ASTVARIABLENODE_HPP
