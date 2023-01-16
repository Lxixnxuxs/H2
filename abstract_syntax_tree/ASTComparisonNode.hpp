//
// Created by ray on 06.12.22.
//

#include "ASTComputationNode.hpp"
#include <map>

#ifndef H2_ASTCOMPARISONNODE_HPP
#define H2_ASTCOMPARISONNODE_HPP

std::map<std::string, std::string> comp_to_jump = {{"==","je"},
                                                   {"!=","jne"},
                                                   {">","jl"},
                                                   {">=","jle"},
                                                   {"<","jg"},
                                                   {"<=","jge"}};

struct ASTComparisonNode : ASTStatementNode {
    ASTComputationNode* left;
    ASTComputationNode* right;

    std::string comp_type;
    std::string reg1;   // two registers to compare
    std::string reg2;

    ASTComparisonNode(ASTComputationNode* left, ASTComputationNode* right, std::string comp_type, std::string reg1,std::string reg2):
            left(left), right(right), comp_type(comp_type), reg1(reg1),reg2(reg2) {
    }

    std::string compile() override {
        std::string code = "";

        code += left->compile();
        code += right->compile();
        code += "cmp "+reg1+", "+reg2+"\n";
        //code += comp_to_jump[comp_type] + " "
        // figuring out the kind of jump is part of the controll-structures job

        return code;
    }

    VirtualMathTerm calculate_complexity() override {
        auto a = VirtualMathTerm(ADDITION);
        a.children.push_back(left->calculate_complexity());
        a.children.push_back(right->calculate_complexity());
        complexity = a;
        return a;
    }

    std::string to_code() override {
        return left->to_code() + comp_type + right->to_code();
    }

    std::string get_class() override { return "Comparison";}
};

#endif //H2_ASTCOMPARISONNODE_HPP
