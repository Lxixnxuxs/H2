//
// Created by ray on 01.12.22.
//

#ifndef H2_LEXER_HPP
#define H2_LEXER_HPP

#include <string>
#include <vector>
#include <list>
#include <iostream>

using std::string, std::vector, std::cout, std::endl, std::list;

// symbols at the front have higher priority. eg in " -> int" we would find a "->" and not a "-"
const vector<string> lexer_symbols = {"->","==",">=","<=","=","(",")","{","}","-","+","*","/",",",";","<",">"};
const vector<string> operator_symbols = {"-","+","*","/"};

list<string>* lexer(string& text) {



    auto res = new list<string>;

    int last_pos = 0;
    int i = 0;

    /*
    auto push = [&](bool is_whitespace){
        if (!(is_whitespace and last_pos == i-1)){ // take only if not a single whitespace
            res->push_back(Parser::s_substring(text,last_pos,i));
            last_pos = i;
        }
    };
    */

    while ( i<text.size()){
        if (std::isspace(text.at(i))){ //split at whitespace
            if (last_pos < i){ // take if there was something before
                res->push_back(text.substr(last_pos,i-last_pos));
            }
            last_pos = i+1; // throw away the whitespace
        } else {
            for (const string symbol : lexer_symbols){
                size_t pos = text.find(symbol,i);
                if (pos == i){
                    if (last_pos < i){ // take only if not empty
                        res->push_back(text.substr(last_pos,i-last_pos));
                    }
                    res->push_back(text.substr(i,symbol.size()));
                    i+=symbol.size();
                    last_pos = i;
                    i--; //neutralize i++
                    break;
                }
            }
        }
        i++;
    }
    return res;
}



#endif //H2_LEXER_HPP
