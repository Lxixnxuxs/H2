#ifndef H2_EXPRESSION_HPP
#define H2_EXPRESSION_HPP

#include <string>

using namespace std;

enum Operation {
    ADD, SUB, DIV, MUL, LIT, VAR, NONE
};

class Expression {
    Expression* a;
    Expression* b;

    Operation op;

    int d;

    string reg;

public:
    Expression(Expression* a, Expression* b, Operation op, string reg, int d = 0):
                a(a), b(b), op(op), d(d), reg(reg) {};

    static string compile(Expression& e) {
        if (e.op == VAR) {
            return "";
        } else if (e.op == LIT) {
            return "movq $" + to_string(e.d) + "," + e.reg + "\n";
        }

        Expression *a=e.a, *b=e.b;
        if (e.a->op == LIT && e.b->op == LIT) {
            int res;
            switch (e.op) {
                case ADD : res = a->d + b->d; break;
                case SUB : res = a->d - b->d; break;
                case MUL : res = a->d * b->d; break;
                case DIV : res = a->d / b->d; break;
            }

            return "movq $" + to_string(res) + "," + e.reg + "\n";
        } else {

            string com = compile(*e.a);
            com += (e.a->op == VAR ? "\n" : "") + compile(*e.b) + (e.b->op == VAR ? "\n" : "");

            string op_assembly;
            switch(e.op) {
                case ADD: op_assembly = "addq"; break;
                case SUB: op_assembly = "subq"; break;
                case MUL: op_assembly = "imulq"; break;
                case DIV: op_assembly = "idivq"; break;
            }

            com += op_assembly + " " + e.a->reg + "," + e.b->reg + "\nmovq " + e.b->reg + "," + e.reg + "\n";

            return com;
        }
    };
};

#endif //H2_EXPRESSION_HPP
