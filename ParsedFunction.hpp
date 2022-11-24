//
// Created by Linus on 23.11.2022.
//

#ifndef H2_PARSEDFUNCTION_HPP
#define H2_PARSEDFUNCTION_HPP

#include <string>
#include <vector>

struct ParsedFunction {
    std::string name;
    std::string arguments;
    std::string type;
    std::string expression_block;

public:
    ParsedFunction(std::string name, std::string arguments, std::string type, std::string expression_block):
        name(name), arguments(arguments), type(type), expression_block(expression_block) {};

};

#endif //H2_PARSEDFUNCTION_HPP
