//
// Created by ray on 19.01.23.
//

#include "ExecutionPath.hpp"
#include "../abstract_syntax_tree/ASTTree.hpp"
#include "../abstract_syntax_tree/ASTWhileLoopNode.hpp"
#include "../abstract_syntax_tree/ASTCallNode.hpp"


ExecutionPath::ExecutionPath(std::vector<variable_name> arguments, std::vector<std::shared_ptr<ASTStatementNode>> commands): commands(commands) {
        variable_order = arguments;

        for (int i = 0; i<arguments.size(); i++) {
            execution_state[arguments[i]] = VirtualMathTerm(get_param(i));
        }

        for (int i = 0; i<arguments.size(); i++) {
            parameter_back_translation[get_param(i)] = arguments[i];
        }
    }

ExecutionPath::ExecutionPath(const ExecutionPath& other) {
        total_complexity = other.total_complexity;
        execution_state = other.execution_state;
        parameter_back_translation = other.parameter_back_translation;
        commands = other.commands;
        condition = other.condition;
        surrendered = other.surrendered;
        variable_order = other.variable_order;
        // alternative branches do not need to be copied
    }



    void ExecutionPath::start() {

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
        if (total_complexity.contains_variable(UNKNOWN_VAR.name)) {surrendered = true;}
        condition.simplify();
    }


    std::vector<ExecutionPath> ExecutionPath::get_all_branches(){
        std::vector<ExecutionPath> res;
        res.push_back(*this);
        for (auto& e : alternative_branches){
            auto lower = e.get_all_branches();
            res.insert(res.end(), lower.begin(), lower.end());
        }
        return res;
    }




    // returns whether a return ended the execution
    bool ExecutionPath::execute(std::shared_ptr<ASTStatementNode> statement_) {
        ASTStatementNode* statement = statement_.get();
        auto cls = statement->get_class();

        // go into deeper level of analysis
        if (cls == "While") {
            auto node = dynamic_cast<ASTWhileLoopNode *>(statement);
            node->former_exec_state = this;
            auto child_complexity = node->get_complexity();
            for (auto p : execution_state) {
                child_complexity.substitude_variable(p.first,p.second);
            }
            total_complexity.children.push_back(child_complexity);


            // after loop, the used variables cannot be inferred to a value, so set them to unknown
            for (auto& var : node->altered_variables) {
                execution_state[var] = UNKNOWN_VAR;
            }
        }

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
            auto casted = dynamic_cast<ASTCallNode*>(statement);
            auto child_complexity = statement->get_complexity();
            for (auto p : execution_state) {
                child_complexity.substitude_variable(p.first,p.second);
            }
            child_complexity.simplify();
            total_complexity.children.push_back(child_complexity);

            // TODO I should substitute in the current values of the arguments, or am I wrong?
        }

        if (cls == "Return" or cls == "Assignment") {
            ASTCalculationNode* calc;
            std::string writeback_name;

            if (cls == "Assignment") {
                auto pr = dynamic_cast<ASTAssignmentNode *>(statement);
                calc = dynamic_cast<ASTCalculationNode *>(pr->right.get());
                writeback_name = pr->var_name;

                // add a new parameter variable
                if (pr->is_declaration){
                    variable_order.push_back(writeback_name);
                    parameter_back_translation[get_param(execution_state.size())] = writeback_name;
                    execution_state[pr->var_name] = VirtualMathTerm(); // declaration

                }

            }
            if (cls == "Return") {
                auto pr = dynamic_cast<ASTReturnNode*>(statement);
                calc = dynamic_cast<ASTCalculationNode*>(pr->calc.get());
            }


            VirtualMathTerm child_complexity = {0};
            if (calc!= nullptr) child_complexity = calc->get_complexity();
            // the child complexity should be in terms of the call-values, so substitute the current state in
            for (auto p : execution_state) {
                child_complexity.substitude_variable(p.first,p.second);
            }

            child_complexity.simplify();
            total_complexity.children.push_back(child_complexity);


            VirtualMathTerm v = {0};
            if (calc!= nullptr) v=calc->get_complexity();
            // substitute in the current values of the variables at this point (in order to always have terms of the calling-arguments)
            for (auto p : execution_state) {
                v.substitude_variable(p.first,p.second);
            }

            // writing to the current variable state
            if (cls == "Assignment"){
                execution_state[writeback_name] = v;
            }

        }

        // simplifying all terms
        for (auto& p : execution_state) {
            p.second.simplify();
        }

        return cls == "Return";
    }

    parameter_name get_param(int i) {return "param_"+std::to_string(i);}





