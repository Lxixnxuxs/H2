//
// Created by ray on 17.01.23.
//

#ifndef H2_EXECUTIONPATH_HPP
#define H2_EXECUTIONPATH_HPP

#include "abstract_syntax_tree/ASTTree.hpp"
#include "LogicTerm.hpp"
//#include "abstract_syntax_tree/ASTCallNode.hpp"

typedef std::string variable_name;
typedef std::string parameter_name;

struct ExecutionPath {

    bool surrendered = false;
    VirtualMathTerm total_complexity = VirtualMathTerm(ADDITION);
    std::map<variable_name, VirtualMathTerm> execution_state;
    std::map<parameter_name, variable_name> parameter_back_translation;
    std::vector<ASTStatementNode*> commands;

    LogicTerm condition = {TRUE};


    ExecutionPath(std::vector<variable_name> arguments, std::vector<ASTStatementNode*> commands): commands(commands) {
        for (int i = 0; i<arguments.size(); i++) {
            execution_state[arguments[i]] = VirtualMathTerm(get_param(i));
        }

        for (int i = 0; i<arguments.size(); i++) {
            parameter_back_translation[get_param(i)] = arguments[i];
        }
    }

    ExecutionPath(const ExecutionPath& other) {
        total_complexity = other.total_complexity;
        execution_state = other.execution_state;
        parameter_back_translation = other.parameter_back_translation;
        commands = other.commands;
        condition = other.condition;
        // alternative branches do not need to be copied
    }

    void start() {
        // should only be called from constructor
        while (!commands.empty() and !surrendered) {
            auto first = commands[0];
            commands.erase(commands.begin());
            if (execute(first)) break;  // end execution at return
        }


        // substitute all parameters back in
        for (int i = 0; i<parameter_back_translation.size(); i++) {
            total_complexity.substitude_variable(get_param(i),
                                                 VirtualMathTerm(parameter_back_translation[get_param(i)]));
        }

        total_complexity.simplify();
        condition.simplify();
    }


    std::vector<ExecutionPath> get_all_branches(){
        std::vector<ExecutionPath> res;
        res.push_back(*this);
        for (auto& e : alternative_branches){
            auto lower = e.get_all_branches();
            res.insert(res.end(), lower.begin(), lower.end());
        }
        return res;
    }

private:
    std::vector<ExecutionPath> alternative_branches;

    // returns whether a return ended the execution
    bool execute(ASTStatementNode* statement) {
        auto cls = statement->get_class();

        // not able to copy with those
        if (cls == "While") surrendered = true;

        // divide into branches
        if (cls == "IfElse") {
            auto node = dynamic_cast<ASTIfElseNode *>(statement);
            ExecutionPath else_executer = *this;

            condition = {L_AND,{condition, node->condition->as_logic_term()}};

            // the current execution continues with the if-body

            commands.insert(commands.begin(),node->if_block.begin(), node->if_block.end());


            // the copy execution continues with the else-body
            else_executer.condition = LogicTerm(L_NOT,{condition});
            else_executer.commands.insert(else_executer.commands.begin(),node->else_block.begin(), node->else_block.end());
            else_executer.start();
            alternative_branches.push_back(else_executer);

        }

        if (cls == "Call") {
            /*auto casted = dynamic_cast<ASTCallNode*>(statement);*/
            auto child_complexity = statement->calculate_complexity();
            child_complexity.simplify();
            total_complexity.children.push_back(child_complexity);
        }

        if (cls == "Return" or cls == "Assignment") {
            ASTCalculationNode* calc;
            std::string writeback_name;

            if (cls == "Assignment") {
                auto pr = dynamic_cast<ASTAssignmentNode *>(statement);
                if (pr->is_declaration) execution_state[pr->var_name] = VirtualMathTerm(); // declaration

                calc = dynamic_cast<ASTCalculationNode *>(pr->right);
                writeback_name = pr->var_name;

            }
            if (cls == "Return") {
                auto pr = dynamic_cast<ASTReturnNode*>(statement);
                calc = dynamic_cast<ASTCalculationNode*>(pr->calc);
            }


            VirtualMathTerm child_complexity = calc->calculate_complexity();
            for (auto p : execution_state) {
                child_complexity.substitude_variable(p.first,p.second);
            }

            child_complexity.simplify();
            total_complexity.children.push_back(child_complexity);

            VirtualMathTerm v = calc->as_math_term();
            for (auto p : execution_state) {
                v.substitude_variable(p.first,p.second);
            }

            // writing to the current variable state
            if (cls == "Assignment") execution_state[writeback_name] = v;

        }

        // simplifying all terms
        for (auto& p : execution_state) {
            p.second.simplify();
        }

        return cls == "Return";
    }

    parameter_name get_param(int i) {return "param_"+std::to_string(i);}

};

#endif //H2_EXECUTIONPATH_HPP
