#ifndef H2_ASTRETURNNODE_HPP
#define H2_ASTRETURNNODE_HPP

#include "ASTStatementNode.hpp"
//#include "ASTCalculationNode.hpp"
struct ASTCalculationNode;


struct ASTReturnNode : ASTStatementNode {

    std::shared_ptr<ASTCalculationNode> calc;
    std::string f_name;

    ASTReturnNode(std::shared_ptr<ASTCalculationNode> calc, std::string f_name);

    std::string compile();

    VirtualMathTerm get_complexity() override;

    std::string to_code() override;

    std::string get_class() override;
};

#endif //H2_ASTRETURNNODE_HPP
