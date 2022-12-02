#include <gtest/gtest.h>
#include "CodeParser.hpp"
#include "abstract_syntax_tree/ASTTree.hpp"


TEST(CODE_PARSER_TEST, PARSE_SIMPLE_LITERAL_CALCULATION) {

    std::list<std::string> calculation = {"(", "3", "*", "4", ")", "+", "(", "10", "-", "3", ")"};
    Tokenstream t(calculation);
    auto l = LocalVariableManager();

    CodeParser parser;

    ASTCalculationNode* calculation_node = parser.parse_calculation(t, l);
}

TEST(CODE_PARSER_TEST, PARSE_LITERAL_VARIABLE_FOUND) {
    auto l = LocalVariableManager();
    l.add_variable("x","int");
    l.add_variable("y","int");

    ASTCalculationNode* res = parse_literal("y",l);
    EXPECT_EQ(res.left, nullptr);
    EXPECT_EQ(res.right, nullptr);
    EXPECT_EQ(res.offset, type_to_size["int"]);


}