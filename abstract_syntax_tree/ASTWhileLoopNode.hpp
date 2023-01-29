#ifndef H2_ASTWHILELOOPNODE_HPP
#define H2_ASTWHILELOOPNODE_HPP

#include "ASTControlFlowNode.hpp"
struct ASTComparisonNode;
struct ASTStatementNode;
//struct ExecutionPath;
#include "../ExecutionPath.hpp"

using std::string;

struct ASTWhileLoopNode : ASTControlFlowNode {
    ASTComparisonNode* condition;
    std::vector<ASTStatementNode*> block;
    int label_id;

    //VirtualMathTerm body_complexity;
    //VirtualMathTerm iterations;

    //bool iteration_complexity_is_custom = false;
    //bool body_complexity_is_custom = false;
    ExecutionPath* initial_execution_state;
    std::vector<std::string> altered_variables;


    ASTWhileLoopNode(ASTComparisonNode* condition, std::vector<ASTStatementNode*> &block,int label_id, std::map<std::string, VirtualMathTerm> complexity_map);


    std::string compile() override;

    VirtualMathTerm calculate_complexity() override;

    std::string to_code() override;

    void set_block_level(int n) override;

    std::string get_class() override;

    void virtual_execution(ExecutionPath higher_level_path);
};

#endif //H2_ASTWHILELOOPNODE_HPP
