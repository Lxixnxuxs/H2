//
// Created by ray on 01.12.22.
//

#ifndef H2_LEXER_HPP
#define H2_LEXER_HPP

#include <string>
#include <vector>
#include <list>
#include <memory>

struct Token;
struct FileEditor;

using std::string;
using std::vector;
using std::list;


void remove_comments(list<string>*);

std::shared_ptr<list<Token>> lexer(FileEditor&);





#endif //H2_LEXER_HPP
