#ifndef H2_ASTFUNCTIONNODE_HPP
#define H2_ASTFUNCTIONNODE_HPP


#include "ASTStatementNode.hpp"
#include "ASTReturnNode.hpp"
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

    // not always will it be possible to figure out the runtime. Therefore the logic has the option to surrender
    bool virtual_exec_surrendered = false;


    int callee_reg_size = 4;
    int callee_reg_count = callee_save_regs.size();

    ASTFunctionNode(std::string f_name, std::vector<ASTStatementNode*> body, size_t f_stack_size, size_t arg_stackpart_size,
                    std::vector<std::pair<std::string,std::string>> argument_list, std::string return_type, std::map<std::string, VirtualMathTerm> complexity_map):
    f_name(f_name), body(body), f_stack_size(f_stack_size), arg_stackpart_size(arg_stackpart_size), argument_list(argument_list), return_type(return_type) {
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
        if (virtual_exec_surrendered) complexity = VirtualMathTerm(f_name); // is its own variable if surrendered
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
        virtual_exec_surrendered = true; // surrendered execpt able to come till the end
        VirtualMathTerm complexity_sum = VirtualMathTerm(ADDITION);

        // dictionary of current state in the execution
        std::map<std::string,VirtualMathTerm> variablename_to_term;
        for (int i = 0; i<argument_list.size(); i++) {
            variablename_to_term[argument_list[i].first] = VirtualMathTerm("param_"+std::to_string(i));
        }
        // all function arguments are set as parameters
        // parameter name -> original name

        std::map<std::string, std::string> parameter;
        for (int i = 0; i<argument_list.size(); i++) {
            parameter["param_"+std::to_string(i)] = argument_list[i].first;
        }


        // ATTENTION: to the parameter name there will be a fitting variable
        // this variable may change its value, but the result will be in terms of the parameters and some numbers




        // execute the body
        for (auto e : body) {
            auto cls = e->get_class();
            cout << cls << endl;


            // not able to copy with those
            if (cls == "While" or cls == "IfElse") return;

            if (cls == "Return" or cls == "Assignment") {
                ASTCalculationNode* calc;
                std::string writeback_name;

                if (cls == "Assignment") {
                    auto pr = dynamic_cast<ASTAssignmentNode *>(e);
                    if (pr->is_declaration) variablename_to_term[pr->var_name] = VirtualMathTerm();

                    calc = dynamic_cast<ASTCalculationNode *>(pr->right);
                    writeback_name = pr->var_name;

                }
                if (cls == "Return") {
                    auto pr = dynamic_cast<ASTReturnNode*>(e);
                    calc = dynamic_cast<ASTCalculationNode*>(pr->calc);
                }


                VirtualMathTerm child_complexity = calc->calculate_complexity();
                for (auto p : variablename_to_term) {
                    child_complexity.substitude_variable(p.first,p.second);
                }

                child_complexity.simplify();
                complexity_sum.children.push_back(child_complexity);

                VirtualMathTerm v = calc->as_math_term();
                for (auto p : variablename_to_term) {
                    v.substitude_variable(p.first,p.second);
                }

                // writing to the current variable state
                if (cls == "Assignment") variablename_to_term[writeback_name] = v;



            }

            // simplifying all terms
            for (auto& p : variablename_to_term) {
                p.second.simplify();
            }
        }

        // put in the original parameter names again
        for (int i = 0; i<argument_list.size(); i++) {
            auto s = "param_"+std::to_string(i);
            complexity_sum.substitude_variable(s, VirtualMathTerm(parameter[s]));
        }



        complexity = complexity_sum;
        virtual_exec_surrendered = false; // successful virtual execution

    }

    std::string get_class() override { return "Function";}
};

#endif //H2_ASTFUNCTIONNODE_HPP
