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



    VirtualMathTerm calculate_complexity() override {

        // managing recursive calls
        if (target->within_active_analysis) {
            std::vector<VirtualMathTerm> math_args;
            for (auto e : arguments) {
                math_args.push_back(e->as_math_term());
            }
            return {target_name, math_args};
        }


        VirtualMathTerm func_complexity = target->complexity; // hopefully already calculated

        // substituting all arguments in
        for (int i = 0; i<target->argument_list.size(); i++) {
            func_complexity.substitude_variable(target->argument_list[i].first, arguments[i]->as_math_term());
        }

        auto res = VirtualMathTerm(ADDITION,{func_complexity});

        for ( auto e : arguments) {
            res.children.push_back(e->calculate_complexity());
        }

        complexity = res;
        return complexity;
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

    std::string get_class() override { return "Call";}

    VirtualMathTerm as_math_term() override {
        return VirtualMathTerm(target_name+"_return", false);
    }
};

#endif //H2_ASTCALLNODE_HPP
