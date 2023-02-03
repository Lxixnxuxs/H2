
#ifndef H2_ASTASSIGNMENTNODE_HPP
#define H2_ASTASSIGNMENTNODE_HPP

#include "ASTStatementNode.hpp"
//#include "ASTCalculationNode.hpp"
struct ASTComputationNode;


struct ASTAssignmentNode : ASTStatementNode {

    size_t offset;
    std::string var_name;
    std::string data_type;
    bool is_declaration;

    ASTComputationNode* right;

    ASTAssignmentNode(size_t offset, ASTComputationNode* right, std::string var_name, std::string data_type = "", bool is_declaraion = false );

    std::string compile() override;

    VirtualMathTerm get_complexity() override;

    std::string to_code() override;

    std::string get_class() override;
};

#endif //H2_ASTASSIGNMENTNODE_HPP