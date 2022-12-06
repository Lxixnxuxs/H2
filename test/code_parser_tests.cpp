#include <gtest/gtest.h>
#include "CodeParser.hpp"
#include "abstract_syntax_tree/ASTTree.hpp"

/*
TEST(CODE_PARSER_TEST, PARSE_SIMPLE_LITERAL_CALCULATION) {

    std::list<std::string> calculation = {"(", "3", "*", "4", ")", "+", "(", "10", "-", "3", ")"};
    Tokenstream t(calculation);
    auto l = LocalVariableManager();

    CodeParser parser;

    ASTCalculationNode* calculation_node = parser.parse_calculation(t, l);
}
 */

TEST(CODE_PARSER_TEST, PARSE_LITERAL_VARIABLE_FOUND) {
    auto l = LocalVariableManager();
    l.add_variable("x","int");
    l.add_variable("y","int");

    CodeParser parser;

    ASTCalculationNode* res = parser.parse_literal("y",l,0);
    EXPECT_EQ(res->left, nullptr);
    EXPECT_EQ(res->right, nullptr);
    EXPECT_EQ(res->offset, type_to_size["int"]);
    EXPECT_EQ(res->reg, regs[0]);
    EXPECT_EQ(res->comp_type, VAR);

}

TEST(CODE_PARSER_TEST, PARSE_LITERAL_VARIABLE_NOT_FOUND) {
    auto l = LocalVariableManager();
    l.add_variable("x","int");
    l.add_variable("y","int");

    CodeParser parser;

    EXPECT_THROW({
        try {
            ASTCalculationNode* res = parser.parse_literal("z",l,0);
        }
        catch(std::invalid_argument e){
            EXPECT_STREQ( "Variable 'z' not defined", e.what());
        }
        },std::invalid_argument);

}



TEST(CODE_PARSER_TEST, PARSE_LITERAL_INT) {
    auto l = LocalVariableManager();
    l.add_variable("x","int");
    l.add_variable("y","int");

    CodeParser parser;

    ASTCalculationNode* res = parser.parse_literal("56",l,0);
    EXPECT_EQ(res->left, nullptr);
    EXPECT_EQ(res->right, nullptr);
    EXPECT_EQ(res->value, 56);
    EXPECT_EQ(res->comp_type, LIT);

}

TEST(CODE_PARSER_TEST, PARSE_LINE) {
    auto l = LocalVariableManager();
    l.add_variable("x","int");
    l.add_variable("y","int");

    CodeParser parser;

    ASTCalculationNode* res = parser.parse_literal("56",l,0);
    EXPECT_EQ(res->left, nullptr);
    EXPECT_EQ(res->right, nullptr);
    EXPECT_EQ(res->value, 56);
    EXPECT_EQ(res->comp_type, LIT);

}