//
// Created by ray on 17.01.23.
//


#ifndef H2_LOGICTERM_HPP
#define H2_LOGICTERM_HPP

#include <vector>

//struct VirtualMathTerm;
#include "virtual_math_term.hpp"


// should there be logic variables?
enum LogicTermType {TRUE, FALSE,    L_AND, L_OR, L_NOT,      GREATER, GREATER_EQUAL, LESS_EQUAL, LESS, EQUAL, UNEQUAL};

struct LogicTerm {
    LogicTermType type;
    std::vector<LogicTerm> logic_children;
    std::vector<VirtualMathTerm> math_children;

    bool is_value() const;
    bool is_combination() const;
    bool is_converter() const;


    LogicTerm(LogicTermType type);

    LogicTerm(LogicTermType type, std::vector<LogicTerm> children);

    LogicTerm(LogicTermType type, std::vector<VirtualMathTerm> children);


    void simplify();

    bool operator==(const LogicTerm& other) const ;

    bool operator!=(const LogicTerm& other) const;

private:
    void copy_to_me(const LogicTerm other);

    void simplify_logic();
};


#endif //H2_LOGICTERM_HPP
