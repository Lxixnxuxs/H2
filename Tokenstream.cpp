//
// Created by ray on 30.11.22.
//

#include <stdexcept>
#include <iostream>
#include <memory>
#include <list>

#include "Tokenstream.hpp"


    bool Tokenstream::empty() const {return begin_ == end_;}

    size_t Tokenstream::size() {
        size_t res = 0;
        auto old_begin = begin_;
        while (!empty()) {
            begin_++;
            res++;
        }

        begin_ = old_begin;
        return res;
    }

    void Tokenstream::operator+=(int x) {std::advance(begin_,x); }

    string Tokenstream::operator*() const {return begin_->content;}

    // only use this constructor if the only purpose is declaring a variable
    Tokenstream::Tokenstream(){}

    Tokenstream::Tokenstream(FileEditor obj): begin_(obj.tokens.begin()), end_(obj.tokens.end()), file_editor(&obj) {}

    Tokenstream::Tokenstream(vector<Token>::iterator begin_, vector<Token>::iterator end_): begin_(begin_), end_(end_) {}

    Tokenstream Tokenstream::read_until(const string& token, bool stop_at_token){
        // gives back whole Tokenstream, if no instance is found

        auto old_begin_ = begin_;
        while(!empty() and operator*() != token) {begin_++;};
        auto res = Tokenstream(old_begin_,begin_);


        if (!empty() and !stop_at_token){
            begin_++;  // throwing away the token just found
        }

        res.file_editor = file_editor;
        return res;
    }

    // returns Tokenstream after the token found (or at the token found, if stop_at_token is true)
    Tokenstream Tokenstream::read_until_one_of(const vector<string>& token_vec, bool stop_at_token){
        auto old_begin_ = begin_;
        bool found = false;
        while(!empty() and !found) {
            for (auto token : token_vec){
                if (operator*() == token) {
                    found = true;
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

        res.file_editor = file_editor;
        return res;
    }

    Tokenstream Tokenstream::read_inside_brackets(){
        auto old_begin_ = begin_;
        string bracket = operator*();
        if (corresponding_bracket.find(bracket) == corresponding_bracket.end()){
            throw std::invalid_argument(file_editor->get_error_position_information(*this)+"'read_inside_brackets' called on non-bracket token: '"+bracket+"'");
        }
        string close_bracket = corresponding_bracket.at(bracket);

        // scanning tokenstream for corresponding bracket
        int counter = 1;
        while(!empty() and counter>0){
            begin_++;
            if (operator*() == bracket) counter++;
            if (operator*() == close_bracket) counter--;
        }


        if (counter != 0) {
            throw std::invalid_argument(file_editor->get_error_position_information(*this)+"'read_inside_brackets' was not able to find corresponding closing bracket of kind: '"+close_bracket+"'");
        }

        auto res = Tokenstream(++old_begin_,    // trowing away opening bracket
                               begin_);
        begin_++;  // throwing away closing bracket
        res.file_editor = file_editor;
        return res;
    }

std::string Tokenstream::to_string() {
    std::string text;
    auto old_begin = begin_;
    while(!empty()) {
        text+=" ";
        text += operator*();
        begin_++;
    }
    begin_ = old_begin;
    return text;
}


Tokenstream Tokenstream::read_while(std::function<bool(string)> predicate) {
        // note that no token is thrown away
    auto old_begin_ = begin_;
    while(!empty() and predicate(operator*())) {begin_++;};
    auto res = Tokenstream(old_begin_,begin_);

    res.file_editor = file_editor;
    return res;
}

Token Tokenstream::get_token() {
    return *begin_;
}
/*
void Tokenstream::delete_token() {
    if (!file_editor) {
        throw std::invalid_argument("tried to modify file editor from tokenstream without file editor");
    }
    auto token = get_token();
    file_editor.value()->enqueue_change(Replacement(token.text_position_boundary->first, token.text_position_boundary->second, ""));

    // go beyond changed position
    operator+=(1);
}

void Tokenstream::insert_token(std::string word) {
    if (!file_editor) {
        throw std::invalid_argument("tried to modify file editor from tokenstream without file editor");
    }
    auto token = get_token();
    file_editor.value()->enqueue_change(Replacement(token.text_position_boundary->first, token.text_position_boundary->first, word+" "));

}*/


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
