//
// Created by ray on 08.12.22.
//

#ifndef H2_GLOBALVARIABLEMANAGER_HPP
#define H2_GLOBALVARIABLEMANAGER_HPP

#include <map>
#include <string>
#include <vector>

struct ASTFunctionNode;


struct GlobalVariableManager {
    std::map<std::string, std::vector<std::pair<std::string,std::string>>> var_to_argument_list; // func_var -> pair(type, arg_name)
    std::map<std::string, ASTFunctionNode*> var_to_node;
};

#endif //H2_GLOBALVARIABLEMANAGER_HPP
