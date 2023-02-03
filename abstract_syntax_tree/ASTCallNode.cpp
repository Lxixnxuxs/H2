//
// Created by ray on 29.01.23.
//
#include "ASTStatementNode.hpp"
#include "ASTFunctionNode.hpp"
#include "ASTCalculationNode.hpp"
#include "ASTCallNode.hpp"

#include <cassert>


ASTCallNode::ASTCallNode(ASTCalculationNode *left, ASTCalculationNode *right, ComputationOp compType, std::string reg, int value,
                size_t offset, ASTFunctionNode* target_, std::vector<ASTCalculationNode*> arguments, int h) : ASTCalculationNode(
            left, right, compType, regs[h], value, offset), target(target_), target_name(target_->f_name), arguments(arguments), h(h) {}
    // call node is always taking register eax


    std::string ASTCallNode::compile() {
        std::string code;
        for (int i = 0; i<arguments.size(); i++) {
            code += arguments[i]->compile();
            code += "mov "+regs[i+h]+", "+argument_regs[i]+"\n";   // put subcalculation-result in corresponding argument place
        }

        code += "call " + target_name + "\n";
        code += "mov %eax, "+regs[h]+"\n";
        return code;
    }

    VirtualMathTerm ASTCallNode::get_complexity() {

        // managing recursive calls
        if (target->within_active_analysis) {
            std::vector<VirtualMathTerm> math_args;
            for (auto e : arguments) {
                math_args.push_back(e->as_math_term());
            }
            return {target->f_name, math_args};
        }


        VirtualMathTerm func_complexity = target->get_complexity(); // hopefully already calculated

        // substituting all arguments in (two steps to prevent errors if func-argument has same name as parameter called with)
        auto get_temporary_name = [](int i){return "temp_"+std::to_string(i);};

        for (int i = 0; i<target->argument_list.size(); i++) {
            auto current_value_in_execution = arguments[i]->as_math_term();
            func_complexity.substitude_variable(target->argument_list[i].first, {get_temporary_name(i)});
            func_complexity.substitude_variable(get_temporary_name(i), current_value_in_execution);
        }

        auto res = VirtualMathTerm(ADDITION,{func_complexity});

        for ( auto e : arguments) {
            res.children.push_back(e->get_complexity());
        }

        complexity = res;
        return complexity;
    }

    std::string ASTCallNode::to_code() {
        auto res =  target_name + "[";
        for (int i = 0; i<arguments.size(); i++) {
            res += arguments[i]->to_code();
            if (i!=arguments.size()-1) res += ", ";
        }
        res += "]";
        return res;

    }

    std::string ASTCallNode::get_class() { return "Call";}

    VirtualMathTerm ASTCallNode::as_math_term() {
        //assert(false); //TODO old code
        return {target_name, (std::vector<VirtualMathTerm>){}};
    }
