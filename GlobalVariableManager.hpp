//
// Created by ray on 08.12.22.
//
#include <map>
#include <string>
#include <vector>

#ifndef H2_GLOBALVARIABLEMANAGER_HPP
#define H2_GLOBALVARIABLEMANAGER_HPP

struct GlobalVariableManager {
    std::map<std::string, std::vector<std::string>> var_to_argument_list;
    std::map<std::string, ASTFunctionNode*> var_to_node;
};

#endif //H2_GLOBALVARIABLEMANAGER_HPP
