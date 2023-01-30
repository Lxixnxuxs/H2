//
// Created by ray on 17.01.23.
//

#ifndef H2_EXECUTIONPATH_HPP
#define H2_EXECUTIONPATH_HPP


//#include "abstract_syntax_tree/ASTCallNode.hpp"

#include <string>
#include <vector>
#include <map>
//#include "abstract_syntax_tree/ASTStatementNode.hpp"
//#include "LogicTerm.hpp"
#include "LogicTerm.hpp"
struct ASTStatementNode;


typedef std::string variable_name;
typedef std::string parameter_name;

struct ExecutionPath {

    bool surrendered = false;
    VirtualMathTerm total_complexity = VirtualMathTerm(ADDITION);

    std::map<variable_name, VirtualMathTerm> execution_state;
    std::map<parameter_name, variable_name> parameter_back_translation;
    std::vector<ASTStatementNode*> commands;
    std::vector<ExecutionPath> alternative_branches;

    LogicTerm condition = {TRUE};


    ExecutionPath(std::vector<variable_name> arguments, std::vector<ASTStatementNode*> commands);

    ExecutionPath(const ExecutionPath& other);

    ExecutionPath()=default;

    void start();

    std::vector<ExecutionPath> get_all_branches();

private:

    // returns whether a return ended the execution
    bool execute(ASTStatementNode* statement);

};

parameter_name get_param(int i);

#endif //H2_EXECUTIONPATH_HPP
