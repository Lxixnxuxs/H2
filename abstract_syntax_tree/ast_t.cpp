#include "ASTComputationNode.hpp"

#include <iostream>

int main() {
    auto e0 = new ASTComputationNode(nullptr, nullptr, LIT, "%rax", 2);
    auto e1 = new ASTComputationNode(nullptr, nullptr, LIT, "%rbx", 3);
    auto e2 = new ASTComputationNode(e0, e1, MUL, "%rcx");
    auto e3 = new ASTComputationNode(nullptr, nullptr, LIT, "%rax", 4);
    auto e4 = new ASTComputationNode(nullptr, nullptr, VAR, "%rbx", 0, 4);
    auto e5 = new ASTComputationNode(e3, e4, SUB, "%rbx");
    ASTNode* e6 = new ASTComputationNode(e2, e5, ADD);

    std::cout << e6->compile();

    return 0;
}
