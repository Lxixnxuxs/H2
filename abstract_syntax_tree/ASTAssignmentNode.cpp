//
// Created by ray on 29.01.23.
//
#include "ASTAssignmentNode.hpp"
#include "ASTComputationNode.hpp"
#include "../global_information.hpp"
#include "ASTVariableNode.hpp"
#include "../LocalVariableManager.hpp"


ASTAssignmentNode::ASTAssignmentNode(size_t offset, std::shared_ptr<ASTComputationNode> right, std::shared_ptr<ASTVariableNode> var, std::string data_type, bool is_declaraion,
                                     int size):
            offset(offset), right(right), var(var), data_type(data_type), is_declaration(is_declaraion), size(size) {}

    std::string ASTAssignmentNode::compile() {
        std::string code;

        // use malloc, if it is a declaration of a class instance
        if (is_declaration and data_type != "int") {
            code += "movl $" + std::to_string(size) + ", %edi\n";
            code += "call malloc\n";
            code += "mov %rax, " + std::to_string(var->local_vars->var_to_offset[var->name]) + "("+ stack_pointer +")\n";
        }

        if (right== nullptr)
            return code;

        code += right->compile();
        code += var->compile();
        code += "mov " + right->reg + ", "+ var->reg+ "\n";
        return code;
    }

    VirtualMathTerm ASTAssignmentNode::get_complexity() {
        auto a = right->get_complexity();
        complexity = a;
        return a;
    }

    std::string ASTAssignmentNode::to_code() {

        return get_indentation(block_level) + (is_declaration ? data_type + " " : "") + var->to_code() + ((right==
                                                                                                   nullptr) ? "": (" = " + right->to_code())) + ";\n";
    }

    std::string ASTAssignmentNode::get_class() { return "Assignment";}
