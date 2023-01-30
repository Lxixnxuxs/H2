#include "ASTReturnNode.hpp"
#include "ASTCalculationNode.hpp"


ASTReturnNode::ASTReturnNode(ASTCalculationNode* calc, std::string f_name): calc(calc), f_name(f_name) {}

    std::string ASTReturnNode::compile() {
        std::string code = calc->compile();
        code += "mov " + calc->reg + ", %eax\n";
        code += "jmp END_" + f_name + '\n';

        return code;
    }

    VirtualMathTerm ASTReturnNode::get_complexity() {
        auto a = calc->get_complexity();
        complexity = a;
        return a;
    }

    std::string ASTReturnNode::to_code() {
        return get_indentation(block_level) + "return " + calc->to_code() + ";\n";
    }

    std::string ASTReturnNode::get_class() { return "Return";}

