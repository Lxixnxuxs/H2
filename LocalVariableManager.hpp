


#ifndef H2_LOCALVARIABLEMANAGER_HPP
#define H2_LOCALVARIABLEMANAGER_HPP

#include <string>
#include <vector>
#include <map>
#include "memory"

struct GlobalVariableManager;

struct LocalVariableManager {
    std::string name;
    std::string ret_type;
    std::vector<std::string> argument_list; // storing the types of the arguments

    std::map<std::string, std::string> var_to_type;
    std::map<std::string,size_t> var_to_offset;
    std::map<std::string, bool> var_to_is_ref;
    int current_offset = 0;

    void add_variable(std::string name, std::string type, const std::shared_ptr<GlobalVariableManager> type_context = nullptr, bool=false);

    bool variable_exists(std::string name);

    LocalVariableManager get_this_namespace(std::shared_ptr<GlobalVariableManager> g);
};

#endif //H2_LOCALVARIABLEMANAGER_HPP