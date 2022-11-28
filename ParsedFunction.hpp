#ifndef H2_PARSEDFUNCTION_HPP
#define H2_PARSEDFUNCTION_HPP

#include <string>
#include <vector>
#include <tuple>

struct ParsedFunction {
    std::string name;
    std::string arguments;
    std::string type;
    std::vector<std::string> statements;
    std::vector<std::pair<std::string, std::string>> local_v;

    ParsedFunction(std::string name, std::string arguments, std::string type, std::vector<std::string> statements):
        name(name), arguments(arguments), type(type), statements(statements) {};

};

#endif //H2_PARSEDFUNCTION_HPP