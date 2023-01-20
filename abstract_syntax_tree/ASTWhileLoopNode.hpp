#ifndef H2_ASTWHILELOOPNODE_HPP
#define H2_ASTWHILELOOPNODE_HPP

#include "ASTControlFlowNode.hpp"
#include "ASTComparisonNode.hpp"
#include "ASTCallNode.hpp"
//#include "ASTFunctionNode.hpp"

using std::string;

struct PseudoFunctionNode {
    std::string f_name;
    bool within_active_analysis = true;
};

struct ASTWhileLoopNode : ASTControlFlowNode {
    ASTComparisonNode* condition;
    std::vector<ASTStatementNode*> block;
    int label_id;

    VirtualMathTerm body_complexity;
    VirtualMathTerm iterations;

    //bool iteration_complexity_is_custom = false;
    //bool body_complexity_is_custom = false;
    ExecutionPath* initial_execution_state;

    ASTWhileLoopNode(ASTComparisonNode* condition, std::vector<ASTStatementNode*> &block,int label_id, std::map<std::string, VirtualMathTerm> complexity_map): condition(condition),
                                                                                                                                                                block(block), label_id(label_id) {
        if (complexity_map.find("O") != complexity_map.end()) {
            complexity_is_custom = true;
            complexity = complexity_map["O"];
            complexity.simplify();
        }
        /*if (complexity_map.find("I") != complexity_map.end()) {
            iteration_complexity_is_custom = true;
            iterations = complexity_map["I"];
            iterations.simplify();
        }
        if (complexity_map.find("C") != complexity_map.end()) {
            body_complexity_is_custom = true;
            body_complexity = complexity_map["C"];
            body_complexity.simplify();
        }*/
    };


    std::string compile() override {
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

    VirtualMathTerm calculate_complexity() override {
        if (!complexity_is_custom) virtual_execution(*initial_execution_state);


        return complexity;
        /*

        auto a = VirtualMathTerm(ADDITION);
        if (!body_complexity_is_custom) {
            for (auto e : block) {
                a.children.push_back(e->calculate_complexity());
            }
            body_complexity = a;
        }
        if (!iteration_complexity_is_custom) {
            iterations = VirtualMathTerm("iter" + std::to_string(label_id));
        }

        if (complexity_is_custom) return complexity;



        a.children.push_back(condition->calculate_complexity());

        // TODO not each iteration may have same complexity, because unknowns change (need parameter logic)



        auto b = VirtualMathTerm(MULTIPLICATION);
        b.children.push_back(iterations);
        b.children.push_back(body_complexity);

        complexity = b;
        return complexity; */
    }

    std::string to_code() override {
        auto res = get_indention(block_level)+"while (" + condition->to_code()+")";
        // TODO: insert O-Notation here
        res += " /% "+(complexity_is_custom ? ((string) "") : ((string) "_"))+"O("+ complexity.as_string() + ") %/ ";
                //+(iteration_complexity_is_custom ? ((string) "") : ((string) "_"))+"I("+iterations.as_string() +") "+
                                //(body_complexity_is_custom ? ((string) "") : ((string) "_"))+"C("+ body_complexity.as_string() +") %/ ";

        res += " {\n";
        for (auto e : block) {
            res += e->to_code();
        }
        res += get_indention(block_level)+"}\n";
        return res;
    }

    void set_block_level(int n) {
        block_level = n;
        for (auto& f: block) f->set_block_level(n+1);
    }

    std::string get_class() override { return "While";}

    void virtual_execution(ExecutionPath higher_level_path) {
        auto logic_condition = condition->as_logic_term();


        // imitate a function
        ASTFunctionNode pseudo_function;
        pseudo_function.f_name = "while_"+ std::to_string(label_id);
        pseudo_function.within_active_analysis = true;
        // okay that argument list is uninitialized?

        // use all variables of the current state as recursion arguments
        std::vector<std::string> variables;
        for (auto p:higher_level_path.execution_state){
            variables.push_back(p.first);
        }


        ExecutionPath base_case = {{},{}};
        base_case.condition = logic_condition;


        auto path = higher_level_path;

        path.total_complexity = VirtualMathTerm(ADDITION);
        path.alternative_branches = {};
        path.commands = block;  // execute the body

        ASTCallNode end_pseudo_call;
        end_pseudo_call.target = &pseudo_function;

        path.commands.push_back(&end_pseudo_call);

        //path.commands.push_back()
        path.condition = {L_NOT, {condition->as_logic_term()}};

        path.start();
        std::vector<ExecutionPath> all_paths = path.get_all_branches();
        all_paths.push_back(base_case);

        for (auto& path : all_paths) {
            if (path.surrendered) return; // surrender if not fully able to execute
        }

        auto analysis_result = analyze_execution_from_all_paths(pseudo_function.f_name,variables,all_paths);
        complexity = analysis_result.second;
    }
};

#endif //H2_ASTWHILELOOPNODE_HPP
