//
// Created by ray on 08.12.22.
//

#include "ASTStatementNode.hpp"
#include "ASTFunctionNode.hpp"
#include "ASTCalculationNode.hpp"
#include "../global_information.hpp"

#ifndef H2_ASTCALLNODE_HPP
#define H2_ASTCALLNODE_HPP

struct ASTCallNode : ASTCalculationNode{
    ASTFunctionNode* target;
    std::string target_name;
    std::vector<ASTCalculationNode*> arguments;
    int h;

    ASTCallNode(ASTCalculationNode *left, ASTCalculationNode *right, ComputationOp compType, std::string reg, int value,
                size_t offset, ASTFunctionNode* target_, std::vector<ASTCalculationNode*> arguments, int h) : ASTCalculationNode(
            left, right, compType, regs[h], value, offset), target(target_), target_name(target_->f_name), arguments(arguments), h(h) {}
            // call node is always taking register eax

    std::string compile() override{
        std::string code;
        for (int i = 0; i<arguments.size(); i++) {
            code += arguments[i]->compile();
            code += "mov "+regs[i+h]+", "+argument_regs[i]+"\n";   // put subcalculation-result in corresponding argument place
        }

        code += "call " + target_name + "\n";
        code += "mov %eax, "+regs[h]+"\n";
        return code;
    }

    ComplexityTerm* calculate_complexity() override {
        // TODO important!!!   the calling cost must be calculated by matching with which arguments the function is called
        //  and how the arguments contribute to the complexity of the function
        //  matching the own function arguments unknowns to the arguments of the called function
        //  (the current implementation is simple but wrong)

        auto* a = target->complexity;
        complexity = a;
        return a;
    }

    std::string to_code() override {
        auto res =  target_name + "[";
        for (int i = 0; i<arguments.size(); i++) {
            res += arguments[i]->to_code();
            if (i!=arguments.size()-1) res += ", ";
        }
        res += "]";
        return res;

    }

};

#endif //H2_ASTCALLNODE_HPP
