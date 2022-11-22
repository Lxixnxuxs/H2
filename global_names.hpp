//
// Created by ray on 22.11.22.
//

#ifndef H2_GLOBAL_NAMES_HPP
#define H2_GLOBAL_NAMES_HPP

#include <string>
#include <vector>

using namespace std;

class Func_block{
public:
    string name;
    vector<string> local_vars;

    Func_block(string name): name(name){
        local_vars = {};
    }
};

#endif //H2_GLOBAL_NAMES_HPP
