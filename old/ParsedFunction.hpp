//
// Created by Linus on 23.11.2022.
//

#ifndef H2_PARSEDFUNCTION_HPP
#define H2_PARSEDFUNCTION_HPP

#include <string>
#include <vector>
#include <map>

struct ParsedFunction {
    std::string name;
    std::string arguments;
    std::string type;
    std::string body;
    //std::vector<std::string> statements;
    //std::vector<std::string> local_v;
    std::map<std::string, std::string> var_to_type;
    std::map<std::string,size_t> var_to_offset;
    int current_offset = 0;


public:
    ParsedFunction(std::string name, std::string arguments, std::string type, std::string body):
        name(name), arguments(arguments), type(type), body(body)  {};

};

#endif //H2_PARSEDFUNCTION_HPP
