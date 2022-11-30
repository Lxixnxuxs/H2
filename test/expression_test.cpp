#include <gtest/gtest.h>
#include <iostream>
#include "old/Expression.hpp"

class ExpressionTest : public :: testing::Test{
protected:

    void SetUp() override {
        e0 = new Expression(nullptr, nullptr, Operation::LIT, "", 5);
        e1 = new Expression(nullptr, nullptr, Operation::LIT, "", 3);
        e2 = new Expression(nullptr, nullptr, Operation::VAR, "%rax");
        e3 = new Expression(e0, e1, Operation::MUL, "%rbx");
        E = new Expression(e3, e2, Operation::ADD, "%rcx");
    }

    void TearDown() override {
        delete e0;
        delete e1;
        delete e2;
        delete e3;
        delete E;
    }

    Expression *e0, *e1, *e2, *e3, *E;
};

TEST_F(ExpressionTest, CorrectlyCompilesExpression) {
    ASSERT_EQ("movl $15,%rbx\naddq %rbx,%rax\nmovq %rax,%rcx", Expression::compile(*E));
}