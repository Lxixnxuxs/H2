//
// Created by Linus on 18.02.2023.
//

#include <cassert>
#include "ASTVariableNode.hpp"
#include "../LocalVariableManager.hpp"
#include "../GlobalVariableManager.hpp"
#include "../global_information.hpp"
#include "../complexity_analyzer/virtual_math_term.hpp"

ASTVariableNode::ASTVariableNode(std::string name, std::shared_ptr<ASTVariableNode> child,
                                 bool is_root, std::shared_ptr<LocalVariableManager> local_vars, std::shared_ptr<GlobalVariableManager> global_vars,
                                 std::string reg, bool return_primitive_by_value):  ASTCalculationNode(nullptr, nullptr,VAR, reg),
                                                                                    name(name), child(child), is_root(is_root), local_vars(local_vars), global_vars(global_vars), return_primitive_by_value(return_primitive_by_value) {
    comp_type = VAR;
};

std::string ASTVariableNode::get_type(std::string last_class_name){
    if (is_root) return local_vars->var_to_type[name];
    else {
        assert(global_vars->class_exists(last_class_name));
        return global_vars->class_to_local_manager[last_class_name]->var_to_type[name];
    }
};

std::string ASTVariableNode::compile_helper(std::string last_class_name="") {



    auto needs_dereferentiation = [&](){
        return get_type(last_class_name)!="int";   //everything but primitive type int need dereferentiation
    };


    std::string code = "";

    if (is_root) { // start: relative to stack-pointer
        code += "mov "+frame_pointer+", " + reg + "\n";
        code += "sub $" + std::to_string(local_vars->var_to_offset[name]) + ", " + reg + "\n";
    } else {  // later: relative to already calculated address. Position in class matters
        code += "sub $" + std::to_string(global_vars->class_to_local_manager[last_class_name]->var_to_offset[name])
                + ", " + reg + "\n";
    }

    if (needs_dereferentiation() and child!=nullptr) code += "mov (" + reg + "), " + reg + "\n";

    // Base Case
    if (child == nullptr) {
        // for primitive types, return actual value instead of address
        //if (get_type() == "int" and return_primitive_by_value) code += "mov (" + reg + "), " + reg + "\n";
        return code;
    }

    code += child->compile_helper(get_type(last_class_name));
    return code;
}

std::string ASTVariableNode::compile() {
    return compile_helper("");
}

std::string ASTVariableNode::get_resulting_type(std::string last_class_name) {
    if (child == nullptr) return get_type(last_class_name);
    else return child->get_resulting_type(get_type(last_class_name));
}

VirtualMathTerm ASTVariableNode::get_complexity() {
    complexity = {1};
    return complexity;
}

std::string ASTVariableNode::to_code() {
    if (child == nullptr) return name;
    else {
        return name + "." + child->to_code();
    }
}


// if this is a primitive type variable, in many cases the value and not a reference to the value is needed
// this function is for unpacking those primitive values, if the given calculation node meets this context
// call to unpack primitive value
void follow_variable_reference_if_applying(std::shared_ptr<ASTComputationNode> node, std::string& code){
    if (node->get_class() == "ASTVariableNode") {
        auto node_var = dynamic_cast<ASTVariableNode *>(node.get());
        //if (node_var->get_resulting_type() == "int") {
        code += "mov (" + node->reg + "), " + node->reg + "\n";

    }
}