#ifndef H2_LOCALVARIABLEMANAGER_HPP
#define H2_LOCALVARIABLEMANAGER_HPP

static std::string data_types[] = {"int"};
static std::map<std::string, size_t> type_to_size = { {"int", 8} };

struct LocalVariableManager {

    std::string name;
    std::string ret_type;

    std::map<std::string, std::string> var_to_type;
    std::map<std::string,size_t> var_to_offset;
    int current_offset = 0;

    void add_variable(std::string name, std::string type){
        var_to_type[name] = type;

        var_to_offset[name] = current_offset;
        current_offset += type_to_size[type];
    }
};

#endif //H2_LOCALVARIABLEMANAGER_HPP
