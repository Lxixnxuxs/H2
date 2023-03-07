//
// Created by ray on 17.02.23.
//

#include "GlobalVariableManager.hpp"
#include "global_information.hpp"

bool GlobalVariableManager::class_exists(std::string name) const {
    return class_to_local_manager.find(name) != class_to_local_manager.end();
}

bool GlobalVariableManager::function_exists(std::string name) const {
    return var_to_argument_list.find(name) != var_to_argument_list.end();
}

int GlobalVariableManager::get_type_representation_size(std::string type) const {
    if (type_to_size.find(type) != type_to_size.end())
        return type_to_size.at(type);

    if (class_exists(type))
        return 8; // 8 bytes for a pointer

        //return class_to_local_manager.at(type)->current_offset;

    throw std::runtime_error("type " + type + " not found");
}
