#ifndef H2_ASTASSIGNMENTNODE_HPP
#define H2_ASTASSIGNMENTNODE_HPP

#include "ASTStatementNode.hpp"
#include "ASTCalculationNode.hpp"

struct ASTAssignmentNode : ASTStatementNode {

    size_t offset;
    std::string var_name;
    std::string data_type;
    bool is_declaration;

    ASTComputationNode* right;

    ASTAssignmentNode(size_t offset, ASTComputationNode* right, std::string var_name, std::string data_type = "", bool is_declaraion = false ):
             offset(offset), right(right), var_name(var_name), data_type(data_type), is_declaration(is_declaraion) {}

    std::string compile() override{
        std::string code = right->compile();
        code += "mov " + right->reg + ", " + std::to_string(offset) + "(%rsp)\n";

        return code;
    }

    VirtualMathTerm calculate_complexity() override {
        auto a = right->calculate_complexity();
        complexity = a;
        return a;
    }

    std::string to_code() override {
        return get_indention(block_level) +(is_declaration ? data_type+" " : "")+  var_name + " = " + right->to_code() + ";\n";
    }

    std::string get_class() override { return "Assignment";}
};

#endif //H2_ASTASSIGNMENTNODE_HPP