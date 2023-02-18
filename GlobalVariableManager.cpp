//
// Created by ray on 17.02.23.
//

#include "GlobalVariableManager.hpp"

bool GlobalVariableManager::class_exists(std::string name) {
    return class_to_local_manager.find(name) != class_to_local_manager.end();
}