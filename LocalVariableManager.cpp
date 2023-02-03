//
// Created by ray on 20.01.23.
//
#include "LocalVariableManager.hpp"
#include "global_information.hpp"

    void LocalVariableManager::add_variable(std::string name, std::string type){
        var_to_type[name] = type;

        var_to_offset[name] = current_offset;
        current_offset += type_to_size[type];
    }
