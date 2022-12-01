//
// Created by ray on 30.11.22.
//

#ifndef H2_TOKENSTREAM_HPP
#define H2_TOKENSTREAM_HPP

#include <list>
#include <string>
#include <iostream>
#include <map>

using std::string, std::cout, std::list, std::map;

map<string,string> corresponding_bracket = {{"(",")"},{"{","}"},{"[","]"},{"<",">"}};

struct Tokenstream {
    list<string>::iterator begin_;
    list<string>::iterator end_;

    //list<string>::iterator begin(){ return begin_;}
    //list<string>::iterator end(){ return end_;}

    bool empty() {return begin_ == end_;}
    void operator+=(int x) {std::advance(begin_,x); }
    void operator++() { begin_++; }
    void operator--() {begin_--;}
    string operator*() {return *begin_;}

    Tokenstream(list<string>* obj): begin_(obj->begin()), end_(obj->end()) {}

    Tokenstream(list<string>::iterator begin_, list<string>::iterator end_): begin_(begin_), end_(end_) {}

    Tokenstream read_until(string token){
        auto old_begin_ = begin_;
        while(begin_!=end_ and *begin_ != token) {begin_++;};
        begin_--;
        auto res = Tokenstream(old_begin_,begin_);
        (begin_++)++;  // throwing away the token just found
    }

    Tokenstream read_inside_brackets(){
        bool found = false;
        string bracket = *begin_;
        string close_bracket;
        for (auto p : corresponding_bracket){
            if (p.first == bracket){
                close_bracket = p.second;
                found = true;
            }
        }
        if (!found) {
            throw "'read_inside_brackets' called on non-bracket token: '"+bracket+"'";
        }

        // TODO implement
    }
};



template <typename a>
std::ostream& operator<<(std::ostream& os, std::list<a>& t){
    for (auto it = t.begin(); it != t.end(); it++){
        os <<  "|";
        os  << *it;
    }
    return os;
}

#endif //H2_TOKENSTREAM_HPP
