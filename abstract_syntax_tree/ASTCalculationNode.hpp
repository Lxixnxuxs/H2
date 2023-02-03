
#ifndef H2_ASTCALCULATIONNODE_HPP
#define H2_ASTCALCULATIONNODE_HPP

#include "ASTComputationNode.hpp"

enum ComputationOp {LIT, VAR, ADD, SUB, MUL, DIV, MOD};

struct ASTCalculationNode : ASTComputationNode {
    ASTCalculationNode* left;
    ASTCalculationNode* right;

    ComputationOp comp_type;

    //std::string reg;
    bool own_reg = true;

    // Optional variables for cases (LIT, VAR)
    int value;
    std::string var_name;
    size_t offset;


    ASTCalculationNode(ASTCalculationNode* left, ASTCalculationNode* right, ComputationOp comp_type, std::string reg = "",
                       int value = 0, size_t offset = 0, std::string var_name = "SomeVar");

    ASTCalculationNode()=default;

    std::string compile() override;

    std::string computation_op_to_string();

    std::string optimize_literal_computation();

    bool operator==(ASTCalculationNode& other);

    VirtualMathTerm get_complexity() override;

    std::string to_code() override;

    std::string get_class() override;

    virtual VirtualMathTerm as_math_term();
};

#endif //H2_ASTCALCULATIONNODE_HPP