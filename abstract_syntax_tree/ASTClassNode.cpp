//
// Created by ray on 17.02.23.
//

#include "ASTClassNode.hpp"
#include "ASTFunctionNode.hpp"
#include "../LocalVariableManager.hpp"

std::string ASTClassNode::compile() {
    std::string code = "; " + name + "\n";
    for (auto method : methods) {
        code += method->compile();
    }
    return code;
}

std::string ASTClassNode::to_code() {
    std::string code = "class " + name;

    // complexity parameters
    code += " [";
    int nr = complexity_parameters.size();
    for (auto p : complexity_parameters) {
        nr--;
        code += p;
        if (nr!=0) code +=", ";

    }
    code += "] ";

    code += " {\n";

    // variables
    for (auto [var_name, type] : object_variables->var_to_type){
        code += get_indentation(block_level+1)  + type + " " + var_name + ";\n";
    }
    code+="\n";

    // methods
    for (auto method : methods) {
        code += method->to_code()+"\n";// + " [] \n";       // todo treat parameters properly
    }
    code += "}\n";
    return code;
}

std::string ASTClassNode::get_class() {
    return "Class";
}

ASTClassNode::ASTClassNode(std::string name, std::vector<std::string> runtime_parameters,
                           std::shared_ptr<LocalVariableManager> object_variables,
                           std::vector<std::shared_ptr<ASTFunctionNode>> methods): name(name),
                           complexity_parameters(runtime_parameters),
                           object_variables(object_variables), methods(methods),
                            size_of(object_variables->current_offset) {}

void ASTClassNode::set_block_level(int n) {
    block_level = n;
    for (const auto& f: methods) f->set_block_level(n+1);
    //for (auto f: object_variables) f->set_block_level(n+1);
}
