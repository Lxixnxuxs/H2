//
// Created by ray on 07.01.23.
//

#include <iostream>
#include <stdexcept>

#include "lexer.hpp"

void remove_comments(list<string>* l){
    auto it = l->begin();
    int comment_active = 0;

    while(it != l->end()){
        string token = *it;

        if (token == "/*"){
            comment_active += 1;
            it = l->erase(it);
        } else if (token == "*/"){
            comment_active -= 1;
            if (comment_active<0) {
                throw std::invalid_argument("LEXER ERROR:  Found '*/' without a corresponding comment-open-symbol '/*'");
            }
            it = l->erase(it);
        } else {
            if (comment_active>=1) {
                it = l->erase(it);
            } else {
                it++;
            }

        }
    }

    if (comment_active) {
        throw std::invalid_argument("LEXER ERROR:  Opened comment with '/*' but never closed it with '*/'");
    }
}

list<string>* lexer(string& text) {
    auto res = new list<string>;

    int last_pos = 0;
    int i = 0;

    while ( i<text.size()){
        if (std::isspace(text.at(i))){ //split at whitespace
            if (last_pos < i){ // take if there was something before
                res->push_back(text.substr(last_pos,i-last_pos));
            }
            last_pos = i+1; // throw away the whitespace
        } else {
            for (const string &symbol : lexer_symbols){
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

    // include last token which would be forgotten otherwise
    if (last_pos < i) {res->push_back(text.substr(last_pos,i-last_pos));}

    remove_comments(res); // comments are removed by default
    return res;
}
