#ifndef H2_ASTFUNCTIONNODE_HPP
#define H2_ASTFUNCTIONNODE_HPP


#include "ASTStatementNode.hpp"
#include <vector>

struct ASTFunctionNode : ASTNode {

    std::string f_name;
    size_t f_stack_size;
    std::vector<ASTStatementNode*> body;

    ASTFunctionNode(std::string f_name, std::vector<ASTStatementNode*> body, size_t f_stack_size):
    f_name(f_name), body(body), f_stack_size(f_stack_size) {}

    std::string compile() {
        std::string code = f_name + ":\n";

        code += "addq $" + std::to_string(f_stack_size) + ", %rsp\n";

        for (ASTStatementNode* e : body) {
            code += e->compile();
        }

        code += "END_" + f_name + ":\n";

        code += "subq $"+std::to_string(f_stack_size) + ", %rsp\nret\n";

        return code;
    }
};

#endif //H2_ASTFUNCTIONNODE_HPP
