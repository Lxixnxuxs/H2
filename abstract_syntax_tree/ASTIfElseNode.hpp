#ifndef H2_ASTIFELSENODE_HPP
#define H2_ASTIFELSENODE_HPP

#include <vector>
#include "ASTControlFlowNode.hpp"
struct ASTComparisonNode;
struct ASTStatementNode;


struct ASTIfElseNode : ASTControlFlowNode {
    std::shared_ptr<ASTComparisonNode> condition;
    std::vector<std::shared_ptr<ASTStatementNode>> if_block;
    std::vector<std::shared_ptr<ASTStatementNode>> else_block;

    int label_id;

    ASTIfElseNode(std::shared_ptr<ASTComparisonNode> condition, std::vector<std::shared_ptr<ASTStatementNode>> &if_block,
                  std::vector<std::shared_ptr<ASTStatementNode>> &else_block, int label_id);

    std::string compile() override;

    VirtualMathTerm get_complexity() override;

    std::string to_code() override;

    void set_block_level(int n);

    std::string get_class() override;
};

#endif //H2_ASTIFELSENODE_HPP
