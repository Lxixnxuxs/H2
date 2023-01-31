//
// Created by ray on 29.01.23.
//

#include "ASTControlFlowNode.hpp"
#include "ASTComparisonNode.hpp"
#include "ASTCallNode.hpp"
#include "ASTFunctionNode.hpp"
#include "../recursive_analysis.hpp"
#include "../ExecutionPath.hpp"

#include "ASTWhileLoopNode.hpp"

using std::string;


ASTWhileLoopNode::ASTWhileLoopNode(ASTComparisonNode* condition, std::vector<ASTStatementNode*> &block,int label_id, std::map<std::string, VirtualMathTerm> complexity_map): condition(condition),
                                                                                                                                                               block(block), label_id(label_id) {
        if (complexity_map.find("O") != complexity_map.end()) {
            complexity_is_custom = true;
            complexity = complexity_map["O"];
            complexity.simplify();
        }
    };


    std::string ASTWhileLoopNode::compile() {
        std::string code = "jmp L_WHILE_COND_"+std::to_string(label_id) +
                           "\nL_WHILE_BODY_"+std::to_string(label_id)+":\n";

        for (auto& node : block) {
            code += node->compile();
        }

        code+= "L_WHILE_COND_"+std::to_string(label_id)+":\n";

        code += condition->compile();

        code += comp_to_jump[condition->comp_type]+" L_WHILE_BODY_" + std::to_string(label_id) + "\n";

        return code;
    }

    std::string ASTWhileLoopNode::to_code() {
        auto res = get_indentation(block_level) + "while (" + condition->to_code() + ")";
        // TODO: insert O-Notation here
        res += " /% "+(complexity_is_custom ? ((string) "") : ((string) "_"))+"O("+ complexity.as_string() + ") %/ ";
        //+(iteration_complexity_is_custom ? ((string) "") : ((string) "_"))+"I("+iterations.as_string() +") "+
        //(body_complexity_is_custom ? ((string) "") : ((string) "_"))+"C("+ body_complexity.as_string() +") %/ ";

        res += " {\n";
        for (auto e : block) {
            res += e->to_code();
        }
        res += get_indentation(block_level) + "}\n";
        return res;
    }

    void ASTWhileLoopNode::set_block_level(int n) {
        block_level = n;
        for (auto& f: block) f->set_block_level(n+1);
    }

    std::string ASTWhileLoopNode::get_class() { return "While";}


VirtualMathTerm ASTWhileLoopNode::get_complexity() {
    if (!complexity_is_custom and !complexity_already_calculated) virtual_execution(*former_exec_state);
    return complexity;
}

void ASTWhileLoopNode::virtual_execution(ExecutionPath higher_level_path) {

        // imitate a function
        ASTFunctionNode pseudo_function;
        pseudo_function.f_name = "while_"+ std::to_string(label_id);
        pseudo_function.within_active_analysis = true;
        // okay that argument list is uninitialized?

        // use all variables of the current state as recursion arguments
        std::vector<std::string> variables = higher_level_path.variable_order;
        /*for (auto p:higher_level_path.execution_state){
            variables.push_back(p.first);
        }*/


        ExecutionPath base_case = {{},{}};
        base_case.condition = {L_NOT, {condition->as_logic_term()}};
        base_case.condition.simplify();


        ExecutionPath path = {variables, block};

        ASTCallNode end_pseudo_call;
        end_pseudo_call.target = &pseudo_function;
        for (int i = 0; i < variables.size(); i++) {
            end_pseudo_call.arguments.push_back(new ASTCalculationNode(nullptr, nullptr, VAR, "", 0, 0, variables[i]));
        }

        path.commands.push_back(&end_pseudo_call);

        path.condition = condition->as_logic_term();

        path.start();
        std::vector<ExecutionPath> all_paths = path.get_all_branches();
        all_paths.push_back(base_case);


        // find out which variables have been altered
        altered_variables = {};
        for (auto& m : all_paths) {
            if (m.surrendered) return; // surrender if not fully able to execute

            for (int j = 0; j<m.variable_order.size(); j++) {
                if (m.execution_state[m.variable_order[j]] != (VirtualMathTerm){get_param(j)}) {
                    altered_variables.push_back(m.variable_order[j]);
                }
                j++;
            }
        }

        auto analysis_result = analyze_execution_from_all_paths(pseudo_function.f_name, variables, all_paths);
        complexity = analysis_result.second;

        /*
        // substitute in the execution state of the outer context
        int j = 0;
        for (auto& e : higher_level_path.execution_state) {
            complexity.substitude_variable(variables[j], higher_level_path.execution_state[variables[j]]);
            j++;
        }
        */

    }
