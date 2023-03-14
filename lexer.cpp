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


