//
// Created by ray on 22.11.22.
//

#ifndef H2_PARSER_HPP
#define H2_PARSER_HPP


#include <vector>
#include <string>
#include <fstream>
#include "global_names.hpp"
#include "assert.h"

using namespace std;


vector<string> datatypes = {"int"};

// printing vectors
template <typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> vec){
    os << "[ ";
    for (T e : vec){
        os << e << "\n";
    }
    os << "]";

    return os;
}

// code copied from stack-overflow:
#include <algorithm>
#include <cctype>
#include <locale>


// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}
// end copied code






vector<string> read_file(const string& filename){
    vector<string> res;
    ifstream f(filename);
    string temp;

    if (!f.good()) cout<<"file not found";

    while(getline(f,temp)){
        trim(temp);     // remove the whitespaces at beginning and end
        res.push_back(temp);
    }

    return res;
}

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

vector<string> split(const string& s, char splitter = ' '){
    vector<string> res;
    string cur;
    for (char b : s) {
        if (b == splitter) {
            if (!cur.empty()) {
                res.push_back(cur);
                cur = "";
            }
        } else {
            cur += b;
        }
    }
    return res;
}

Func_block parse_func(vector<string>& func){
    auto res = Func_block("-");
    string head = func[0];
    int state = 0;
    string cur;

    for (char b : head) {
        switch (state) {
            case 0:
                if (b==' '){
                    state = 1;
                    cur = "";
                }
                break;


            case 1:
                if (b=='('){
                    state = 2;
                    res.name = cur;
                    cur = "";
                } else if (b!=' '){
                    cur+=b;
                }

                break;

            case 2:

        }
    }

/*
    if(head[0]!="def"){
        cout << "ERROR func declaration has to start with 'def' " << endl;
    }
    head.erase(head.begin());
    cout << head << endl;
*/

    for (int i = 1; i<func.size(); i++){

        // finding local variables
        for (string& d : datatypes){
            if (d.length()<=func[i].length() and d == func[i].substr(0,d.length())){
                cout << "found: " << func[i] << endl;
            }
        }
    }
    return ;
}

#endif //H2_PARSER_HPP
