//
// Created by ray on 30.11.22.
//

#ifndef H2_TOKENSTREAM_HPP
#define H2_TOKENSTREAM_HPP

#include <list>
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <optional>
#include <memory>

#include "FileEditor.hpp"

using std::string;
using std::list;
using std::map;
using std::vector;


static const map<string,string> corresponding_bracket = {{"(",")"},{"{","}"},{"[","]"},{"/%","%/"},{"/*","*/"},
                                                         {"'","'"}, {"\"","\""}};

struct Tokenstream {
    vector<Token>::iterator begin_;
    vector<Token>::iterator end_;

    FileEditor* file_editor;

    bool empty() const;
    size_t size();
    void operator+=(int);
    string operator*() const;
    Token get_token();

    // only use this constructor if the only purpose is declaring a variable
    Tokenstream();

    Tokenstream(FileEditor obj);

    Tokenstream(vector<Token>::iterator begin_, vector<Token>::iterator end_);

    Tokenstream read_while(std::function<bool(string)> predicate);

    Tokenstream read_until(const string& token, bool stop_at_token = false);

    Tokenstream read_until_one_of(const vector<string>& token_vec, bool stop_at_token = false);

    Tokenstream read_inside_brackets();

    std::string to_string();

    void delete_token();

    void insert_token(std::string word);

    /*void delete_tokenstream();

    void insert_tokenstream(Tokenstream other);*/
};


template <typename a>
std::ostream& operator<<(std::ostream& os, std::list<a>& t);

std::ostream& operator<<(std::ostream& os, Tokenstream& t);

#endif //H2_TOKENSTREAM_HPP
