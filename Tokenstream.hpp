//
// Created by ray on 30.11.22.
//

#ifndef H2_TOKENSTREAM_HPP
#define H2_TOKENSTREAM_HPP

#include <list>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <stdexcept>

using std::string, std::cout, std::list, std::map, std::vector, std::endl;

map<string,string> corresponding_bracket = {{"(",")"},{"{","}"},{"[","]"}};

struct Tokenstream {
    list<string>::iterator begin_;
    list<string>::iterator end_;

    //list<string>::iterator begin(){ return begin_;}
    //list<string>::iterator end(){ return end_;}

    bool empty() {return begin_ == end_;}
    size_t size() {
        size_t res = 0;
        list<string>::iterator old_begin = begin_;
        while (!empty()) {
            begin_++;
            res++;
        }

        begin_ = old_begin;
        return res;
    }

    void operator+=(int x) {std::advance(begin_,x); }
    /*Tokenstream& operator++() { std::advance(begin_,1);
    return *this;}
    void operator--() {begin_--;}*/
    string operator*() {return *begin_;}

    // only use this constructor if the only purpose is declaring a variable
    Tokenstream(){}

    Tokenstream(list<string>* obj): begin_(obj->begin()), end_(obj->end()) {}

    Tokenstream(list<string>::iterator begin_, list<string>::iterator end_): begin_(begin_), end_(end_) {}

    Tokenstream read_until(string token){
        // given back whole Tokenstream, if no instance is found

        auto old_begin_ = begin_;
        while(!empty() and *begin_ != token) {begin_++;};
        auto res = Tokenstream(old_begin_,begin_);

        if (!empty()){
            begin_++;  // throwing away the token just found
        }
        return res;
    }

    Tokenstream read_until_one_of(vector<string> token_vec){
        auto old_begin_ = begin_;
        bool found = false;
        while(!empty() and !found) {
            for (auto token : token_vec){
                if (*begin_ == token) {found = true;}
            }
            begin_++;
        };

        Tokenstream res;

        if (!empty()){
            begin_--; // neutralize last 'begin_++' in the loop, when 'found' has already become true
            res = Tokenstream(old_begin_,begin_);
            begin_++;  // throwing away the token just found
        } else {
            // special treatment of no instance is found
            res = Tokenstream(old_begin_,begin_);
        }



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
                break;
            }
        }


        if (close_bracket.empty()) {
            throw std::invalid_argument("'read_inside_brackets' called on non-bracket token: '"+bracket+"'");
        }

        // scanning tokenstream for corresponding bracket
        int counter = 1;
        while(!empty() and counter>0){
            begin_++;
            if (*begin_ == bracket) counter++;
            if (*begin_ == close_bracket) counter--;
        }


        if (counter != 0) {
            throw std::invalid_argument("'read_inside_brackets' was not able to find corresponding closing bracket of kind: '"+close_bracket+"'");
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
