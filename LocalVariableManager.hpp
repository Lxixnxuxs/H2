


#ifndef H2_LOCALVARIABLEMANAGER_HPP
#define H2_LOCALVARIABLEMANAGER_HPP

#include <string>
#include <vector>
#include <map>

struct LocalVariableManager {
    std::string name;
    std::string ret_type;
    std::vector<std::string> argument_list; // storing the types of the arguments

    std::map<std::string, std::string> var_to_type;
    std::map<std::string,size_t> var_to_offset;
    int current_offset = 0;

    void add_variable(std::string name, std::string type);
};

#endif //H2_LOCALVARIABLEMANAGER_HPP
