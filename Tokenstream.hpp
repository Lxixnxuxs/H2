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
    void operator++() { std::advance(begin_,1); }
    void operator--() {begin_--;}
    string operator*() {return *begin_;}

    explicit Tokenstream(list<string>* obj): begin_(obj->begin()), end_(obj->end()) {}

    Tokenstream(list<string>::iterator begin_, list<string>::iterator end_): begin_(begin_), end_(end_) {}

    Tokenstream read_until(string token){
        auto old_begin_ = begin_;
        while(!empty() and *begin_ != token) {begin_++;};
        auto res = Tokenstream(old_begin_,begin_);
        begin_++;  // throwing away the token just found
        return res;
    }

    Tokenstream read_inside_brackets(){
        auto old_begin_ = begin_;
        string bracket = *begin_;
        string close_bracket = "";

        // figure out closing-bracket symbol
        for (auto p : corresponding_bracket){
            if (p.first == bracket){
                close_bracket = p.second;
            }
        }
        if (close_bracket.empty()) {
            throw "'read_inside_brackets' called on non-bracket token: '"+bracket+"'";
        }

        // scanning tokenstream for corresponding bracket
        int counter = 1;
        while(!empty() and counter>0){
            begin_++;
            if (*begin_ == bracket) counter++;
            if (*begin_ == close_bracket) counter--;
        }

        auto res = Tokenstream(++old_begin_,    // trowing away opening bracket
                               begin_);
        begin_++;  // throwing away closing bracket
        return res;
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


std::ostream& operator<<(std::ostream& os, Tokenstream& t){
    auto old_begin = t.begin_;
    while(!t.empty()) {
        os <<  "|";
        os  << *t;
        t+=1;
    }
    t.begin_ = old_begin;
    return os;
}

#endif //H2_TOKENSTREAM_HPP
