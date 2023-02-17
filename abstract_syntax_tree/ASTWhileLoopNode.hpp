#ifndef H2_ASTWHILELOOPNODE_HPP
#define H2_ASTWHILELOOPNODE_HPP

#include "ASTControlFlowNode.hpp"
struct ASTComparisonNode;
struct ASTStatementNode;
//struct ExecutionPath;
#include "../complexity_analyzer/ExecutionPath.hpp"

using std::string;

struct ASTWhileLoopNode : ASTControlFlowNode {
    ASTComparisonNode* condition;
    std::vector<ASTStatementNode*> block;
    int label_id;

    ExecutionPath* former_exec_state; // execution of the parent-function until the point before while-loop started
    std::vector<std::string> altered_variables;


    ASTWhileLoopNode(ASTComparisonNode* condition, std::vector<ASTStatementNode*> &block,int label_id, std::map<std::string, VirtualMathTerm> complexity_map);


    std::string compile() override;

    VirtualMathTerm get_complexity() override;

    std::string to_code() override;

    void set_block_level(int n) override;

    std::string get_class() override;

private:
    void virtual_execution(ExecutionPath higher_level_path);
};

#endif //H2_ASTWHILELOOPNODE_HPP
