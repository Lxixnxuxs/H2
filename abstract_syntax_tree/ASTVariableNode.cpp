//
// Created by Linus on 18.02.2023.
//

#include <cassert>
#include "ASTVariableNode.hpp"
#include "../LocalVariableManager.hpp"
#include "../GlobalVariableManager.hpp"
#include "../global_information.hpp"


std::string ASTVariableNode::compile_helper(std::string last_class_name="") {

    auto get_type = [&](){
        if (is_root) return local_vars->var_to_type[name];
        else {
            assert(global_vars->class_exists(last_class_name));
            return global_vars->class_to_local_manager[last_class_name]->var_to_type[name];
        }
    };

    auto needs_dereferentiation = [&](){
        return get_type()!="int";   //everything but primitive type int need dereferentiation
    };


    std::string code = "";

    if (is_root) { // start: relative to stack-pointer
        code += "mov "+frame_pointer+", " + reg + "\n";
        code += "sub $" + std::to_string(local_vars->var_to_offset[name]) + ", " + reg + "\n";
    } else {  // later: relative to already calculated address. Position in class matters
        code += "sub $" + std::to_string(global_vars->class_to_local_manager[last_class_name]->var_to_offset[name])
                + ", " + reg + "\n";
    }

    if (needs_dereferentiation()) code += "mov (" + reg + "), " + reg + "\n";

    // Base Case
    if (child == nullptr) {
        // for primitive types, return actual value instead of address
        if (get_type() == "int") code += "mov (" + reg + "), " + reg + "\n";
        return code;
    }

    code += child->compile_helper(get_type());
    return code;
}

std::string ASTVariableNode::compile() {
    return compile_helper("");
}

ASTVariableNode::ASTVariableNode(std::string name, std::shared_ptr<ASTVariableNode> child,
bool is_root, std::shared_ptr<LocalVariableManager> local_vars, std::shared_ptr<GlobalVariableManager> global_vars,
        std::string reg):  ASTCalculationNode(nullptr, nullptr,VAR, reg),
        name(name), child(child), is_root(is_root), local_vars(local_vars), global_vars(global_vars) {
    comp_type = VAR;
};