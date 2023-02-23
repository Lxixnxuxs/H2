//
// Created by Linus on 18.02.2023.
//

#include "ASTVariableNode.hpp"
#include "../LocalVariableManager.hpp"
#include "../GlobalVariableManager.hpp"
#include "../global_information.hpp"


std::string ASTVariableNode::compile(std::string last_class_name="") {
    if (is_primitive) {
        return "implement primitive types";
    }

    std::string code = "";

    if (is_root) {
        code += "mov "+stack_pointer+", " + reg + "\n";
        code += "add $" + std::to_string(local_vars->var_to_offset[name]) + ", " + reg + "\n";
    }

    if (child == nullptr) {
        if (!local_vars->var_to_is_ref[name])
            code += "mov (" + reg + "), " + reg + "\n";
    } else {
        if (local_vars->var_to_is_ref[name]) {
            code += "mov (" + reg + "), " + reg + "\n";
        } else {
            code += "add $" + std::to_string(global_vars->class_to_local_manager[last_class_name]->var_to_offset[name])
                    + ", " + reg + "\n";

            code += child->compile(local_vars->var_to_type[name]);
        }
    }

    return code;
}

std::string ASTVariableNode::compile() {
    return compile("");
}
