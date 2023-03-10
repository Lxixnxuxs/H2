//
// Created by ray on 29.01.23.
//

#include "ASTComparisonNode.hpp"
#include "ASTComputationNode.hpp"
#include "../complexity_analyzer/LogicTerm.hpp"
#include "ASTVariableNode.hpp"




std::map<std::string, std::string> comp_to_jump = {{"==","je"},
                                                   {"!=","jne"},
                                                   {">","jl"},
                                                   {">=","jle"},
                                                   {"<","jg"},
                                                   {"<=","jge"}};

std::map<std::string, LogicTermType> comp_to_logic_type = {{"==",EQUAL},
                                                           {"!=",UNEQUAL},
                                                           {">",GREATER},
                                                           {">=",GREATER_EQUAL},
                                                           {"<",LESS},
                                                           {"<=",LESS_EQUAL}};



ASTComparisonNode::ASTComparisonNode(std::shared_ptr<ASTComputationNode> left, std::shared_ptr<ASTComputationNode> right, std::string comp_type, std::string reg1,std::string reg2):
            left(left), right(right), comp_type(comp_type), reg1(reg1),reg2(reg2) {
    }

    std::string ASTComparisonNode::compile() {
        std::string code = "";

        code += left->compile();
        follow_variable_reference_if_applying(left,code);
        code += right->compile();
        follow_variable_reference_if_applying(right,code);
        code += "cmp "+reg1+", "+reg2+"\n";
        //code += comp_to_jump[comp_type] + " "
        // figuring out the kind of jump is part of the controll-structures job

        return code;
    }

    VirtualMathTerm ASTComparisonNode::get_complexity() {
        auto a = VirtualMathTerm(ADDITION);
        a.children.push_back(left->get_complexity());
        a.children.push_back(right->get_complexity());
        complexity = a;
        return a;
    }

    std::string ASTComparisonNode::to_code() {
        return left->to_code() + comp_type + right->to_code();
    }

    std::string ASTComparisonNode::get_class() { return "Comparison";}

    LogicTerm ASTComparisonNode::as_logic_term() {
        return {comp_to_logic_type[comp_type], {left->as_math_term(), right->as_math_term()}};
    }
