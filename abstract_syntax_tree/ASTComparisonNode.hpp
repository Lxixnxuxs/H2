//
// Created by ray on 06.12.22.
//

#ifndef H2_ASTCOMPARISONNODE_HPP
#define H2_ASTCOMPARISONNODE_HPP

#include <map>
#include "ASTStatementNode.hpp"
#include "../complexity_analyzer/LogicTerm.hpp"

struct ASTComputationNode;


extern std::map<std::string, std::string> comp_to_jump;
extern std::map<std::string, LogicTermType> comp_to_logic_type;


struct ASTComparisonNode : ASTStatementNode {
    std::shared_ptr<ASTComputationNode> left;
    std::shared_ptr<ASTComputationNode> right;

    std::string comp_type;
    std::string reg1;   // two registers to compare
    std::string reg2;

    ASTComparisonNode(std::shared_ptr<ASTComputationNode> left, std::shared_ptr<ASTComputationNode> right, std::string comp_type, std::string reg1,std::string reg2);

    std::string compile() override;

    VirtualMathTerm get_complexity() override;

    std::string to_code() override;

    std::string get_class() override;

    LogicTerm as_logic_term();
};

#endif //H2_ASTCOMPARISONNODE_HPP
