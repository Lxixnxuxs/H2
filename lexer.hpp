//
// Created by ray on 01.12.22.
//

#ifndef H2_LEXER_HPP
#define H2_LEXER_HPP

#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <stdexcept>

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::list;

// symbols at the front have higher priority. eg in " -> int" we would find a "->" and not a "-"
const vector<string> lexer_symbols = {"/*","*/","/%","%/","->","==","!=",">=","<=","=","(",")","[","]","{","}","-","+","*","/",",",";","<",">","%"};

void remove_comments(list<string>*);

list<string>* lexer(string&);





#endif //H2_LEXER_HPP
