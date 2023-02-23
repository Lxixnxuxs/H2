//
// Created by ray on 17.02.23.
//

#include "GlobalVariableManager.hpp"

bool GlobalVariableManager::class_exists(std::string name) const {
    return class_to_local_manager.find(name) != class_to_local_manager.end();
}

bool GlobalVariableManager::function_exists(std::string name) const {
    return var_to_argument_list.find(name) != var_to_argument_list.end();
}
