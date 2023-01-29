#ifndef H2_ASTRETURNNODE_HPP
#define H2_ASTRETURNNODE_HPP

#include "ASTStatementNode.hpp"
//#include "ASTCalculationNode.hpp"
struct ASTCalculationNode;


struct ASTReturnNode : ASTStatementNode {

    ASTCalculationNode* calc;
    std::string f_name;

    ASTReturnNode(ASTCalculationNode* calc, std::string f_name);

    std::string compile();

    VirtualMathTerm calculate_complexity() override;

    std::string to_code() override;

    std::string get_class() override;
};

#endif //H2_ASTRETURNNODE_HPP
