//
// Created by ray on 20.01.23.
//
#include "LocalVariableManager.hpp"
#include "global_information.hpp"
#include "GlobalVariableManager.hpp"

    void LocalVariableManager::add_variable(std::string name, std::string type, GlobalVariableManager* type_context,
                                            bool is_ref){
        var_to_type[name] = type;
        var_to_is_ref[name] = is_ref;

        var_to_offset[name] = current_offset;
        if (type_context!= nullptr and type_context->class_exists(type)) {
            current_offset += type_context->class_to_local_manager[type].current_offset;
        } else {
            current_offset += type_to_size[type];
        }

    }

    bool LocalVariableManager::variable_exists(std::string name) {
        return var_to_type.find(name) != var_to_type.end();
    }

LocalVariableManager LocalVariableManager::get_this_namespace(GlobalVariableManager &g) {
    if (variable_exists("this")) return g.class_to_local_manager[var_to_type["this"]];
    return {}; // return empty
}
