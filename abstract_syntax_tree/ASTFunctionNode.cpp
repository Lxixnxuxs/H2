//
// Created by ray on 29.01.23.
//



#include "ASTStatementNode.hpp"
#include "ASTReturnNode.hpp"
#include <vector>
#include "../global_information.hpp"
#include "../virtual_math_term.hpp"

//extern struct ExecutionPath;
#include "../ExecutionPath.hpp"
#include "../recursive_analysis.hpp"


#include "ASTFunctionNode.hpp"


ASTFunctionNode::ASTFunctionNode(std::string f_name, std::vector<ASTStatementNode*> body, size_t f_stack_size, size_t arg_stackpart_size,
                    std::vector<std::pair<std::string,std::string>> argument_list, std::string return_type, std::map<std::string, VirtualMathTerm> complexity_map):
            f_name(f_name), body(body), f_stack_size(f_stack_size), arg_stackpart_size(arg_stackpart_size), argument_list(argument_list), return_type(return_type) {
        initialize_with_complexity_map(complexity_map);
    }


    void ASTFunctionNode::initialize_with_complexity_map(std::map<std::string, VirtualMathTerm> complexity_map) {
        if (complexity_map.find("O") != complexity_map.end()) {
            complexity_is_custom = true;
            complexity = complexity_map["O"];
            complexity.simplify();
        }
    }

    std::string ASTFunctionNode::compile() {
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

    std::string ASTFunctionNode::to_code() {
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

    void ASTFunctionNode::set_block_level(int n) {
        block_level = n;
        for (auto& f: body) f->set_block_level(n+1);
    }

VirtualMathTerm ASTFunctionNode::get_complexity() {
    if (!complexity_is_custom and !complexity_already_calculated) {
        within_active_analysis = true;
        virtual_execution();
        within_active_analysis = false;
    }
    if (virtual_exec_surrendered) complexity = VirtualMathTerm("func_"+f_name); // is its own variable if surrendered
    return complexity;

}

    void ASTFunctionNode::virtual_execution() {
        virtual_exec_surrendered = true; // surrendered except able to come to the end
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

        auto analysis_result = analyze_execution_from_all_paths(f_name, args, all_paths);
        virtual_exec_surrendered = !analysis_result;
        if (!analysis_result) return; // surrender

        complexity = analysis_result.value();

    }

    std::string ASTFunctionNode::get_class() { return "Function";}
