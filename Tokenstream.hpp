//
// Created by ray on 30.11.22.
//

#ifndef H2_TOKENSTREAM_HPP
#define H2_TOKENSTREAM_HPP

#include <list>
#include <string>
#include <map>
#include <vector>


using std::string;
using std::list;
using std::map;
using std::vector;


static const map<string,string> corresponding_bracket = {{"(",")"},{"{","}"},{"[","]"},{"/%","%/"},{"/*","*/"}};

struct Tokenstream {
    list<string>::iterator begin_;
    list<string>::iterator end_;

    bool empty() const;
    size_t size();
    void operator+=(int);
    string operator*();

    // only use this constructor if the only purpose is declaring a variable
    Tokenstream();

    Tokenstream(list<string>* obj);

    Tokenstream(list<string>::iterator begin_, list<string>::iterator end_);

    Tokenstream read_until(const string& token);

    std::pair<Tokenstream, std::string> read_until_one_of(const vector<string>& token_vec);

    Tokenstream read_inside_brackets();

    std::string to_string();
};


template <typename a>
std::ostream& operator<<(std::ostream& os, std::list<a>& t);

std::ostream& operator<<(std::ostream& os, Tokenstream& t);

#endif //H2_TOKENSTREAM_HPP
