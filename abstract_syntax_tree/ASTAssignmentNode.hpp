
#ifndef H2_ASTASSIGNMENTNODE_HPP
#define H2_ASTASSIGNMENTNODE_HPP

#include "ASTStatementNode.hpp"
//#include "ASTCalculationNode.hpp"
struct ASTComputationNode;
struct ASTVariableNode;
struct LocalVariableManager;

struct ASTAssignmentNode : ASTStatementNode {

    size_t offset;

    int size;

    std::shared_ptr<ASTVariableNode> var;
    std::string data_type;
    bool is_declaration;

    std::shared_ptr<ASTComputationNode> right;

    ASTAssignmentNode(size_t offset, std::shared_ptr<ASTComputationNode> right, std::shared_ptr<ASTVariableNode> var, std::string data_type = "", bool is_declaraion = false,
                      int size=0);

    std::string compile() override;

    VirtualMathTerm get_complexity() override;

    std::string to_code() override;

    std::string get_class() override;
};

#endif //H2_ASTASSIGNMENTNODE_HPP