//
// Created by ray on 30.11.22.
//

#include <stdexcept>
#include <iostream>

#include "Tokenstream.hpp"


    bool Tokenstream::empty() const {return begin_ == end_;}

    size_t Tokenstream::size() {
        size_t res = 0;
        list<string>::iterator old_begin = begin_;
        while (!empty()) {
            begin_++;
            res++;
        }

        begin_ = old_begin;
        return res;
    }

    void Tokenstream::operator+=(int x) {std::advance(begin_,x); }

    /*Tokenstream& operator++() { std::advance(begin_,1);
    return *this;}
    void operator--() {begin_--;}*/

    string Tokenstream::operator*() {return *begin_;}

    // only use this constructor if the only purpose is declaring a variable
    Tokenstream::Tokenstream(){}

    Tokenstream::Tokenstream(list<string>* obj): begin_(obj->begin()), end_(obj->end()) {}

    Tokenstream::Tokenstream(list<string>::iterator begin_, list<string>::iterator end_): begin_(begin_), end_(end_) {}

    Tokenstream Tokenstream::read_until(const string& token, bool stop_at_token){
        // gives back whole Tokenstream, if no instance is found

        auto old_begin_ = begin_;
        while(!empty() and *begin_ != token) {begin_++;};
        auto res = Tokenstream(old_begin_,begin_);

        if (!empty() and !stop_at_token){
            begin_++;  // throwing away the token just found
        }
        return res;
    }

    // returns Tokenstream after the token found (or at the token found, if stop_at_token is true)
    Tokenstream Tokenstream::read_until_one_of(const vector<string>& token_vec, bool stop_at_token){
        auto old_begin_ = begin_;
        bool found = false;
        string token_found = "";
        while(!empty() and !found) {
            for (auto token : token_vec){
                if (*begin_ == token) {
                    found = true;
                    token_found = *begin_;
                }
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

        if (stop_at_token and found) begin_--;

        return res;
    }

    Tokenstream Tokenstream::read_inside_brackets(){
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

std::string Tokenstream::to_string() {
    std::string text;
    auto old_begin = begin_;
    while(!empty()) {
        text+=" ";
        text += *begin_;
        begin_++;
    }
    begin_ = old_begin;
    return text;
}


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
