//
// Created by Linus on 22.11.2022.
//

#ifndef H2_EXPRESSION_HPP
#define H2_EXPRESSION_HPP

#include <string>

using namespace std;

enum Operation {
    ADD, SUB, DIV, MUL, LIT, VAR
};

class Expression {
    Expression* a;
    Expression* b;

    Operation op;

    int d;

    string reg;

public:
    Expression(Expression* a, Expression* b, Operation op, string& reg, int d = 0):
                a(a), b(b), op(op), d(d), reg(reg) {};


    static string compile(Expression& e) {
        Expression *a=e.a, *b=e.b;
        if (e.a->op == LIT && e.b->op == LIT) {
            int res;
            switch (e.op) {
                case ADD : res = a->d + b->d;
                case SUB : res = a->d - b->d;
                case MUL : res = a->d * b->d;
                case DIV : res = a->d / b->d;
            }

            return "movl $" + to_string(res) + "," + e.reg;
        } else {
            string com = compile(*e.a) + "\n" + compile(*e.b);

            string op_assembly;
            switch(e.op) {
                case ADD: op_assembly = "addq";
                case SUB: op_assembly = "subq";
                case MUL: op_assembly = "imulq";
                case DIV: op_assembly = "idivq";
            }

            com += "\n " + op_assembly + " " + e.a->reg + "," + e.b->reg + "\n movq " + e.b->reg + e.reg;

            return com;
        }
    };
};

#endif //H2_EXPRESSION_HPP
