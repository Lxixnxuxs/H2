//
// Created by ray on 20.01.23.
//
#include "LocalVariableManager.hpp"
#include "global_information.hpp"
#include "GlobalVariableManager.hpp"
#include <cassert>

    void LocalVariableManager::add_variable(std::string name, std::string type, const std::shared_ptr<GlobalVariableManager> type_context,
                                            bool is_ref){
        var_to_type[name] = type;
        var_to_is_ref[name] = is_ref;

        var_to_offset[name] = current_offset;
        if (is_ref) {
            current_offset += type_to_size["int"]; // int should be 64 bit and will be used as an address
            return;
        }

        if (type == "int") {    // primitive types (int is only primitive)
            current_offset += type_to_size["int"];
            return;
        }

        // prevent wrong calling conditions
        assert(type_context != nullptr);
        assert(type_context->class_exists(type));
        current_offset += type_context->class_to_local_manager.at(type)->current_offset;
    }

    bool LocalVariableManager::variable_exists(std::string name) {
        return var_to_type.find(name) != var_to_type.end();
    }

std::shared_ptr<LocalVariableManager> LocalVariableManager::get_this_namespace(std::shared_ptr<GlobalVariableManager> g) {
    if (variable_exists("this")) return g->class_to_local_manager[var_to_type["this"]];
    return {}; // return empty
}
