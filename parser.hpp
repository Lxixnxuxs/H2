//
// Created by ray on 22.11.22.
//

#ifndef H2_PARSER_HPP
#define H2_PARSER_HPP


#include <vector>
#include <string>
#include <fstream>

using namespace std;




vector<vector<string>> split_functions(const vector<string>& s){
    int func_count = 0;

    // list of functions. Each func replesented by rows
    vector<vector<string>> res;
    for (const string& l : s){

        // new func found
        if (l.substr(0,3)=="def"){
            res.push_back(vector<string>());
            func_count++;
        }

        res[func_count-1].push_back(l);
    }
    return res;
}

vector<string> read_file(const string& filename){
    vector<string> res;
    fstream f(filename);
    string temp;
    while(getline(f,temp)){
        res.push_back(temp);
    }

    return res;
}


#endif //H2_PARSER_HPP
