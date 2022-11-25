#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <vector>
#include "parser.hpp"

class ParserTest : public :: testing::Test {
protected:

    void SetUp() override {
        #define STR1(x) #x
        #define STR2(x) STR1(x)

        std::string test_file_path = STR2(TEST_FILE_PATH);

        std::ifstream test_file_stream(test_file_path);

        parser.read(test_file_stream);

        parsed_funcs = parser.split_by_functions();
    }

    void TearDown() override {}

    Parser parser;
    std::vector<ParsedFunction> parsed_funcs;
};

TEST_F(ParserTest, CorrectlyDetectedNumberFunctions) {
    ASSERT_EQ(parsed_funcs.size(), 2);
}

TEST_F(ParserTest, CorrectlyParsesFirstFunction) {
    EXPECT_EQ(parsed_funcs[0].name, "f");
    EXPECT_EQ(parsed_funcs[0].arguments, "int x");
    EXPECT_EQ(parsed_funcs[0].type, "int");
    EXPECT_EQ(parsed_funcs[0].expression_block, "{\r\n    int a = x;\r\n    return 2 * a;\r\n}");
}

TEST_F(ParserTest, CorrectlyParsesMainFunction) {
    EXPECT_EQ(parsed_funcs[1].name, "main");
    EXPECT_EQ(parsed_funcs[1].arguments, "");
    EXPECT_EQ(parsed_funcs[1].type, "void");
    EXPECT_EQ(parsed_funcs[1].expression_block, "{\r\n    return ((7 * 5) + 6);\r\n}");
}