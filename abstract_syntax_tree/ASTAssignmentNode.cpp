//
// Created by ray on 29.01.23.
//
#include "ASTAssignmentNode.hpp"
#include "ASTComputationNode.hpp"
#include "../global_information.hpp"


ASTAssignmentNode::ASTAssignmentNode(size_t offset, std::shared_ptr<ASTComputationNode> right, std::string var_name, std::string data_type, bool is_declaraion):
            offset(offset), right(right), var_name(var_name), data_type(data_type), is_declaration(is_declaraion) {}

    std::string ASTAssignmentNode::compile() {
        if (right== nullptr)return "";

        std::string code = right->compile();
        code += "mov " + right->reg + ", " + std::to_string(offset) + "("+stack_pointer+")\n";
        return code;
    }

    VirtualMathTerm ASTAssignmentNode::get_complexity() {
        auto a = right->get_complexity();
        complexity = a;
        return a;
    }

    std::string ASTAssignmentNode::to_code() {

        return get_indentation(block_level) + (is_declaration ? data_type + " " : "") + var_name + ((right==
                                                                                                   nullptr) ? "": (" = " + right->to_code())) + ";\n";
    }

    std::string ASTAssignmentNode::get_class() { return "Assignment";}
