//
// Created by ray on 29.01.23.
//



#include "ASTStatementNode.hpp"
#include "ASTReturnNode.hpp"
#include <vector>
#include "../global_information.hpp"
#include "../complexity_analyzer/virtual_math_term.hpp"
#include "../LocalVariableManager.hpp"

//extern struct ExecutionPath;
#include "../complexity_analyzer/ExecutionPath.hpp"
#include "../complexity_analyzer/recursive_analysis.hpp"


#include "ASTFunctionNode.hpp"


ASTFunctionNode::ASTFunctionNode(std::string f_name, std::vector<std::shared_ptr<ASTStatementNode>> body, std::shared_ptr<LocalVariableManager> stack_frame, size_t arg_stackpart_size,
                    std::vector<std::pair<std::string,std::string>> argument_list, std::string return_type, std::optional<std::string> class_name, std::map<std::string, VirtualMathTerm> complexity_map):
            f_name(f_name), body(body), stack_frame(stack_frame), arg_stackpart_size(arg_stackpart_size), return_type(return_type),
            class_name(class_name) {

    this->argument_list = argument_list;

    /*if (class_name) {
        this->argument_list.insert(this->argument_list.begin(), {"this", class_name.value()});
    }*/

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

        code += "push %rbp\nmov %rsp, %rbp\n";
        code += "sub $" + std::to_string(stack_frame->current_offset + callee_reg_count*callee_reg_size) + ", "+stack_pointer+"\n";

        // storing all the callee-save register
        for (int i = 0; i<callee_reg_count; i++) {
            code += "mov " + callee_save_regs[i]+ ", -" + std::to_string(stack_frame->current_offset + i*callee_reg_size) +"("+frame_pointer+")\n";
        }

        // moving arguments to stack
        auto current_var =stack_frame->var_to_offset.begin();
        for (int i = 0; i<argument_list.size(); i++) {      // reading the offsets of the first elements in stack_frame
            code += "mov " + argument_regs[i]+ ", -" + std::to_string( current_var->second) +"("+frame_pointer+")\n";
            current_var++;
        }

        for (auto e : body) {
            code += e->compile();
        }

        code += "END_" + f_name + ":\n";

        // restoring all the callee-save register

        for (int i = 0; i<callee_reg_count; i++){
            code += "mov -" + std::to_string(stack_frame->current_offset + i*callee_reg_size) +"("+frame_pointer+"), "+ callee_save_regs[i] +"\n";
        }

        code += "add $"+std::to_string(stack_frame->current_offset + callee_reg_count * callee_reg_size) + ", "+stack_pointer+"\n"
                                                                                                               "pop %rbp\n"
                                                                                                               "ret\n\n\n";

        return code;
    }

    std::string ASTFunctionNode::to_code() {
        auto res = get_indentation(block_level)+"def " + f_name + "(";
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

        res += get_indentation(block_level)+"}\n";

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
