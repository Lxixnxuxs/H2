//
// Created by ray on 29.01.23.
//
#include "ASTCalculationNode.hpp"
#include <stdexcept>


ASTCalculationNode::ASTCalculationNode(ASTCalculationNode* left, ASTCalculationNode* right, ComputationOp comp_type, std::string reg,
                       int value, size_t offset, std::string var_name): left(left), right(right), comp_type(comp_type), value(value), offset(offset), var_name(var_name){
        this->reg = reg;
        if (reg.empty() && left != nullptr) {reg = left->reg; own_reg = false;}
    }


    std::string ASTCalculationNode::compile() {
        if (comp_type == VAR) {
            return "mov " + std::to_string(offset) + "(%rsp)" + ", " + reg + "\n";
        } else if (comp_type == LIT) {
            return "mov $" + std::to_string(value) + "," + reg + "\n";
        } else {
            // Recursive code generation
            if (left->comp_type == LIT && right->comp_type == LIT) {
                return "mov $" + optimize_literal_computation() + ", " + reg + "\n";
            } else {
                std::string code = left->compile() + right->compile();
                if (comp_type == DIV) {
                    /*code += "mov " + left->reg + ", %eax\nmov " + right->reg + ", %edx\n" +
                            "div " + (own_reg ? reg : left->reg) + "\n";*/
                    code += "mov $0, %edx\nmov " + left->reg + ", %eax\ndiv " + right->reg +
                            (own_reg ? "\nmov %eax, " + left->reg : "\nmov %eax, " + reg) + "\n";
                }else if (comp_type == MOD) {
                    code += "mov $0, %edx\nmov " + left->reg + ", %eax\ndiv " + right->reg +
                            (own_reg ? "\nmov %edx, " + left->reg : "\nmov %edx, " + reg) + "\n";
                } else {
                    code += computation_op_to_string() + " " + right->reg + "," + left->reg +
                            (own_reg ? "\nmov " + left->reg + "," + reg + "\n" : "\n");
                }

                return code;
            }
        }
    }

    std::string ASTCalculationNode::computation_op_to_string() {
        switch (comp_type) {
            case ADD: return "add";
            case SUB: return "sub";
            case MUL: return "mul";
            case DIV: return "div";
        }
        return "";
    }

    std::string ASTCalculationNode::optimize_literal_computation() {
        switch(comp_type) {
            case ADD: return std::to_string(left->value + right->value);
            case SUB: return std::to_string(left->value - right->value);
            case MUL: return std::to_string(left->value * right->value);
            case DIV: return std::to_string(left->value / right->value);
        }
        return "";
    }

    bool ASTCalculationNode::operator==(ASTCalculationNode& other) {
        if (comp_type == LIT) {
            return other.comp_type == LIT && other.value == value && other.reg == reg;
        } else if (comp_type == VAR) {
            return other.comp_type == VAR && other.offset == offset && other.reg == reg;
        } else {
            bool b0 = other.comp_type == comp_type;
            bool b1 = other.reg == reg;
            bool b2 = other.left == left;
            bool b3 = other.right == right;

            return false;
            //return other.comp_type == comp_type && other.reg == reg && other.left == left && other.right == right;
        }
    }

    VirtualMathTerm ASTCalculationNode::get_complexity() {
        if (comp_type == LIT or comp_type == VAR){
            complexity = VirtualMathTerm(1);
            return complexity;
        }

        auto a = VirtualMathTerm(ADDITION);
        a.children.push_back(left->get_complexity());
        a.children.push_back(right->get_complexity());
        complexity = a;
        return complexity;
    }

    std::string ASTCalculationNode::to_code() {
        switch (comp_type) {
            case LIT: return std::to_string(value);
            case VAR: return var_name;
            case ADD: return "(" + left->to_code() + " + " + right->to_code() + ")";
            case SUB: return "(" + left->to_code() + " - " + right->to_code() + ")";
            case MUL: return "(" + left->to_code() + " * " + right->to_code() + ")";
            case DIV: return "(" + left->to_code() + " / " + right->to_code() + ")";
            case MOD: return "(" + left->to_code() + " % " + right->to_code() + ")";
        }
        return "";
    }

    std::string ASTCalculationNode::get_class() { return "Calculation";}

    VirtualMathTerm ASTCalculationNode::as_math_term() {
        if (comp_type == LIT) return VirtualMathTerm(value, false);
        if (comp_type == VAR) return VirtualMathTerm(var_name, false);

        auto left_term = left->as_math_term();
        auto right_term = right->as_math_term();

        if (comp_type == ADD) {
            auto res =  VirtualMathTerm(ADDITION, {left_term,right_term}, false);
            return res;
            // TODO what is the problem here???
        }
        if (comp_type == SUB) return VirtualMathTerm(ADDITION,
                                                     {left_term, VirtualMathTerm(MULTIPLICATION,{VirtualMathTerm(-1),right_term},
                                                                                 false)},
                                                     false);
        if (comp_type == MUL) return VirtualMathTerm(MULTIPLICATION, {left_term, right_term}, false);
        //if (comp_type == DIV) return VirtualMathTerm(MULTIPLICATION, {left_term, })

        if (comp_type == DIV) return VirtualMathTerm(MULTIPLICATION, {left_term,
                                                                      VirtualMathTerm(EXPONENTIAL,{right_term,-1},false)}, false);

        throw std::invalid_argument("Operation not jet convertible to math term");
    }
