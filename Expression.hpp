//
// Created by Linus on 22.11.2022.
//

#ifndef H2_EXPRESSION_HPP
#define H2_EXPRESSION_HPP

#include "string"

using namespace std;

enum Operation {
    ADD, SUB, DIV, MUL, LIT, VAR;
};

class Expression {
    Expression* a;
    Expression* b;

    Operation* op;

    int d;

    string reg;

public:
    Expression(Expression* a, Expression* b, Operation* op, int d = 0, string& reg = "rax"):
                a(a), b(b), op(op), d(d), reg(reg) {};


    static string compile(Expression* e) {
        return "";
    };
};

#endif //H2_EXPRESSION_HPP
