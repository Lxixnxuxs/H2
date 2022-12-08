#ifndef H2_ASTFUNCTIONNODE_HPP
#define H2_ASTFUNCTIONNODE_HPP


#include "ASTStatementNode.hpp"
#include <vector>
#include "../global_information.hpp"

struct ASTFunctionNode : ASTNode {

    std::string f_name;
    size_t f_stack_size;
    std::vector<ASTStatementNode*> body;
    std::vector<> argument_list;

    int callee_reg_size = 8;
    int callee_reg_count = callee_save_regs.size();

    ASTFunctionNode(std::string f_name, std::vector<ASTStatementNode*> body, size_t f_stack_size):
    f_name(f_name), body(body), f_stack_size(f_stack_size) {}

    std::string compile() {
        std::string code = f_name + ":\n";

        code += "addq $" + std::to_string(f_stack_size + callee_reg_count*callee_reg_size) + ", %rsp\n";

        // storing all the callee-save register
        for (int i = 0; i<callee_reg_count; i++){
            code += "movq " + callee_save_regs[i]+ " -" + std::to_string(f_stack_size + i*callee_reg_size) +"(%rsp)\n";
        }

        for (ASTStatementNode* e : body) {
            code += e->compile();
        }

        code += "END_" + f_name + ":\n";

        // restoring all the callee-save register
        for (int i = 0; i<callee_reg_count; i++){
            code += "movq -" + std::to_string(f_stack_size + i*callee_reg_size) +"(%rsp) "+ callee_save_regs[i] +"\n";
        }

        code += "subq $"+std::to_string(f_stack_size + callee_reg_count * callee_reg_size) + ", %rsp\nret\n";

        return code;
    }
};

#endif //H2_ASTFUNCTIONNODE_HPP
