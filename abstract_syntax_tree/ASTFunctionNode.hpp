#ifndef H2_ASTFUNCTIONNODE_HPP
#define H2_ASTFUNCTIONNODE_HPP


#include "ASTStatementNode.hpp"
#include <vector>
#include "../global_information.hpp"
#include "../virtual_math_term.hpp"

struct ASTFunctionNode : ASTNode {

    std::string f_name;
    size_t f_stack_size;
    size_t arg_stackpart_size;
    std::vector<ASTStatementNode*> body;
    std::vector<std::pair<std::string,std::string>> argument_list;  // name to type
    std::string return_type;


    int callee_reg_size = 4;
    int callee_reg_count = callee_save_regs.size();

    ASTFunctionNode(std::string f_name, std::vector<ASTStatementNode*> body, size_t f_stack_size, size_t arg_stackpart_size,
                    std::vector<std::pair<std::string,std::string>> argument_list, std::string return_type, std::map<std::string, VirtualMathTerm> complexity_map):
    f_name(f_name), body(body), f_stack_size(f_stack_size), arg_stackpart_size(arg_stackpart_size), argument_list(argument_list), return_type(return_type) {
        if (complexity_map.find("O") != complexity_map.end()) {
            complexity_is_custom = true;
            complexity = complexity_map["O"];
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
        if (complexity_is_custom) return complexity;

        auto a = VirtualMathTerm(ADDITION);
        for (auto e : body) {
            a.children.push_back(e->calculate_complexity());
        }
        complexity = a;
        return a;
    }

    std::string to_code() override {
        auto res = "def " + f_name + "(";
        for (int i = 0; i<argument_list.size(); i++) {
            res += argument_list[i].second + " " + argument_list[i].first;
            if (i!=argument_list.size()-1) res += ", ";
        }
        res += ") -> " + return_type;

        // TODO insert O-Notation here
        res += " /% " + (complexity_is_custom ? ((string) "") : ((string) "_")) + "O("+ complexity.as_string() +") %/ ";

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

    void virtual_execution() {
        std::vector<std::string> parameter;
        for (auto p : argument_list) {
            parameter.push_back(p.first);
        }
        std::map<std::string,VirtualMathTerm> variablename_to_term;

    }
};

#endif //H2_ASTFUNCTIONNODE_HPP
