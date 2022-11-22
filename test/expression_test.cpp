#include "Expression.hpp"
#include <iostream>

int main() {
    Expression e0(nullptr, nullptr, Operation::LIT, "", 5);
    Expression e1(nullptr, nullptr, Operation::LIT, "", 3);
    Expression e2(nullptr, nullptr, Operation::VAR, "%rax");
    Expression e3(&e0, &e1, Operation::MUL, "%rbx");
    Expression e4(&e3, &e2, Operation::ADD, "%rcx");

    std::cout << Expression::compile(e4);
}