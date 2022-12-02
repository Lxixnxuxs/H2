#include "ASTCalculationNode.hpp"
#include "ASTIfElseNode.hpp"

#include <iostream>

int main() {
    auto e0 = new ASTCalculationNode(nullptr, nullptr, LIT, "%rax", 2);
    auto e1 = new ASTCalculationNode(nullptr, nullptr, LIT, "%rbx", 3);
    auto e2 = new ASTCalculationNode(e0, e1, MUL, "%rcx");
    auto e3 = new ASTCalculationNode(nullptr, nullptr, LIT, "%rax", 4);
    auto e4 = new ASTCalculationNode(nullptr, nullptr, VAR, "%rbx", 0, 4);
    auto e5 = new ASTCalculationNode(e3, e4, SUB, "%rbx");
    auto e6 = new ASTCalculationNode(e2, e5, SUB);

    ASTStatementNode* e7 = new ASTCalculationNode(nullptr, nullptr, LIT, "%rax", 1);
    ASTStatementNode* e8 = new ASTCalculationNode(nullptr, nullptr, LIT, "%rax", 0);

    std::vector<ASTStatementNode*> if_b;
    std::vector<ASTStatementNode*> else_b;
    if_b.push_back(e7);
    else_b.push_back(e8);

    ASTNode* e9 = new ASTIfElseNode(e6, if_b, else_b, 0);

    std::cout << e9->compile();

    return 0;
}