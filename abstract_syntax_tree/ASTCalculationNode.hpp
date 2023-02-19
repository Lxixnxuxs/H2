
#ifndef H2_ASTCALCULATIONNODE_HPP
#define H2_ASTCALCULATIONNODE_HPP

#include "ASTComputationNode.hpp"

enum ComputationOp {LIT, VAR, ADD, SUB, MUL, DIV, MOD, BITWISE_AND, BITWISE_OR, BITWISE_XOR, SHIFT_L, SHIFT_R};

struct ASTCalculationNode : ASTComputationNode {
    std::shared_ptr<ASTCalculationNode> left;
    std::shared_ptr<ASTCalculationNode> right;

    ComputationOp comp_type;

    //std::string reg;
    bool own_reg = true;

    // Optional variables for cases (LIT, VAR) and (SHIFT_L, SHIFT_R)
    int value;
    std::string var_name;
    size_t offset;


    ASTCalculationNode(std::shared_ptr<ASTCalculationNode> left, std::shared_ptr<ASTCalculationNode> right, ComputationOp comp_type, std::string reg = "",
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

    std::pair<std::string, bool> get_return_type();
};

#endif //H2_ASTCALCULATIONNODE_HPP