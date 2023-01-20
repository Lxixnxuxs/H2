#ifndef H2_ASTFUNCTIONNODE_HPP
#define H2_ASTFUNCTIONNODE_HPP


#include "ASTStatementNode.hpp"
#include "ASTReturnNode.hpp"
#include <vector>
#include "../global_information.hpp"
#include "../virtual_math_term.hpp"

//extern struct ExecutionPath;
#include "../ExecutionPath.hpp"
#include "../recursive_analysis.hpp"


struct ASTFunctionNode : ASTNode {

    std::string f_name;
    size_t f_stack_size;
    size_t arg_stackpart_size;
    std::vector<ASTStatementNode*> body;
    std::vector<std::pair<std::string,std::string>> argument_list;  // name to type
    std::string return_type;

    // not always will it be possible to figure out the runtime. Therefore the logic has the option to surrender
    bool virtual_exec_surrendered = false;
    bool within_active_analysis = false; // mark which functions do not have a closed form yet


    int callee_reg_size = 4;
    int callee_reg_count = callee_save_regs.size();

    ASTFunctionNode(std::string f_name, std::vector<ASTStatementNode*> body, size_t f_stack_size, size_t arg_stackpart_size,
                    std::vector<std::pair<std::string,std::string>> argument_list, std::string return_type, std::map<std::string, VirtualMathTerm> complexity_map):
    f_name(f_name), body(body), f_stack_size(f_stack_size), arg_stackpart_size(arg_stackpart_size), argument_list(argument_list), return_type(return_type) {
        initialize_with_complexity_map(complexity_map);
    }

    ASTFunctionNode()=default;

    void initialize_with_complexity_map(std::map<std::string, VirtualMathTerm> complexity_map) {
        if (complexity_map.find("O") != complexity_map.end()) {
            complexity_is_custom = true;
            complexity = complexity_map["O"];
            complexity.simplify();
        }
    }

    std::string compile() {
        std::string code = f_name + ":\n";

        code += "sub $" + std::to_string(f_stack_size + callee_reg_count*callee_reg_size) + ", %rsp\n";

        // storing all the callee-save register
        for (int i = 0; i<callee_reg_count; i++){
            code += "mov " + callee_save_regs[i]+ ", " + std::to_string(f_stack_size + i*callee_reg_size) +"(%rsp)\n";
        }

        // moving arguments to stack
        for (int i = 0; i<argument_list.size(); i++) {
            code += "mov " + argument_regs[i]+ ", " + std::to_string( i*callee_reg_size) +"(%rsp)\n"; // f_stack_size - arg_stackpart_size +
        }

        for (ASTStatementNode* e : body) {
            code += e->compile();
        }

        code += "END_" + f_name + ":\n";

        // restoring all the callee-save register

        for (int i = 0; i<callee_reg_count; i++){
            code += "mov " + std::to_string(f_stack_size + i*callee_reg_size) +"(%rsp), "+ callee_save_regs[i] +"\n";
        }

        code += "add $"+std::to_string(f_stack_size + callee_reg_count * callee_reg_size) + ", %rsp\nret\n\n\n";

        return code;
    }

    VirtualMathTerm calculate_complexity() override {
        if (!complexity_is_custom) virtual_execution();
        if (virtual_exec_surrendered) complexity = VirtualMathTerm("func_"+f_name); // is its own variable if surrendered
        return complexity;
        /*if (complexity_is_custom) return complexity;

        auto a = VirtualMathTerm(ADDITION);
        for (auto e : body) {
            a.children.push_back(e->calculate_complexity());
        }
        complexity = a;
        return a;*/
    }

    std::string to_code() override {
        auto res = "def " + f_name + "(";
        for (int i = 0; i<argument_list.size(); i++) {
            res += argument_list[i].second + " " + argument_list[i].first;
            if (i!=argument_list.size()-1) res += ", ";
        }
        res += ") -> " + return_type;

        // TODO insert O-Notation here
        res += " /% " + (complexity_is_custom ? ((std::string) "") : ((std::string) "_")) + "O("+ complexity.as_string() +") %/ ";

        res += " {\n";

        for (auto e : body) {
            res += e->to_code();
        }

        res += "}\n";

        return res;
    }

    void set_block_level(int n) {
        block_level = n;
        for (auto& f: body) f->set_block_level(n+1);
    }

    // wrapper function
    void virtual_execution() {
        within_active_analysis = true;
        virtual_execution_logic();
        within_active_analysis = false;
    }

    void virtual_execution_logic() {
        virtual_exec_surrendered = true; // surrendered execpt able to come to the end
        std::vector<std::string> args;
        for (const auto& p : argument_list) {
            args.push_back(p.first);
        }

        ExecutionPath path = {args, body};
        path.start();
        std::vector<ExecutionPath> all_paths = path.get_all_branches();

        for (auto& path : all_paths) {
            if (path.surrendered) return; // surrender if not fully able to execute
        }

        std::vector<std::pair<LogicTerm, VirtualMathTerm>> base_cases;
        std::vector<std::tuple<LogicTerm, VirtualMathTerm, std::vector<VirtualMathTerm>>> recursive_executions;
        for (auto &e: all_paths) {
            auto calls = e.total_complexity.find_calls();
            if (calls.empty()) {
                base_cases.emplace_back(e.condition, e.total_complexity);
            } else {
                recursive_executions.emplace_back(e.condition, e.total_complexity, calls);
            }
        }


            auto analysis_result = analyze_execution_paths(f_name, args, base_cases, recursive_executions);
            //if (!analysis_result.first) return; // surrender

            complexity = analysis_result.second;

            /*
            // not recursive
            vector<VirtualMathTerm> complexities;
            for (auto d : all_paths){
                complexities.push_back(d.total_complexity);
            }
            complexity = VirtualMathTerm(ADDITION,complexities);
            */

        virtual_exec_surrendered = !analysis_result.first; // successful virtual execution

    }

    std::string get_class() override { return "Function";}
};

#endif //H2_ASTFUNCTIONNODE_HPP
