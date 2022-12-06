//
// Created by ray on 01.12.22.
//
#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <vector>
#include "lexer.hpp"

class LexerTest : public :: testing::Test {
protected:

    void SetUp() override {

    }

    void TearDown() override {}


};

TEST_F(LexerTest, t1) {
    string prog1 = "def main() -> void {\n"
                   "    int x = 10;\n"
                   "\n"
                   "    x = (2) * (x);\n"
                   "    if ((x) - (20)) {\n"
                   "        return 10;\n"
                   "    } else {\n"
                   "        return 20;\n"
                   "    }\n"
                   "}";

    list<string> expected = {"def", "main","(",")","->","void","{",
                             "int","x","=","10",";",
                             "x","=","(","2",")","*","(","x",")",";",
                             "if","(","(","x",")","-","(","20",")",")","{",
                             "return","10",";",
                             "}","else","{",
                             "return","20",";","}",
                             "}"};

    list<string> res = *lexer(prog1);
    EXPECT_EQ(expected.size(),res.size());

    auto it_res = res.begin();
    auto it_exp = expected.begin();

    for (int i = 0; i<expected.size(); i++){
        //cout << *it_exp << "|";
        ASSERT_EQ(*it_exp,*it_res);
        it_res++;
        it_exp++;
    }

}

TEST_F(LexerTest, t2) {
    string prog2 = "def fu(int x, int y) -> int {\n"
                   "    return (x) *(y) ;\n"
                   "}";

    list<string> expected = {"def","fu","(","int","x",",","int","y",")","->","int","{",
                             "return","(","x",")","*","(","y",")",";",
                             "}"};

    list<string> res = *lexer(prog2);
    EXPECT_EQ(expected.size(),res.size());

    auto it_res = res.begin();
    auto it_exp = expected.begin();

    for (int i = 0; i<expected.size(); i++){
        ASSERT_EQ(*it_exp,*it_res);
        it_res++;
        it_exp++;
    }
}

TEST_F(LexerTest, edge_last_sign) {
    string prog2 = "def fu(int x, int y) -> int {\n"
                   "    return (x) *(y) ;\n"
                   "} a";

    list<string> expected = {"def","fu","(","int","x",",","int","y",")","->","int","{",
                             "return","(","x",")","*","(","y",")",";",
                             "}","a"};

    list<string> res = *lexer(prog2);
    EXPECT_EQ(expected.size(),res.size());

    auto it_res = res.begin();
    auto it_exp = expected.begin();

    for (int i = 0; i<expected.size(); i++){
        ASSERT_EQ(*it_exp,*it_res);
        it_res++;
        it_exp++;
    }
}

TEST_F(LexerTest, edge) {
    string prog2 = "def y(    int q)-> void {}\n"
                   "\n"
                   "def very_long_func(int xxx)->int{if(0>xxx){return xxx}}";

    list<string> expected = {"def","y","(","int","q",")","->","void","{","}",
                             "def","very_long_func","(","int","xxx",")","->","int","{","if","(","0",">","xxx",")","{","return","xxx","}","}"};

    list<string> res = *lexer(prog2);
    EXPECT_EQ(expected.size(),res.size());

    auto it_res = res.begin();
    auto it_exp = expected.begin();

    for (int i = 0; i<expected.size(); i++){
        ASSERT_EQ(*it_exp,*it_res);
        it_res++;
        it_exp++;
    }
}

TEST_F(LexerTest, comments1) {
    list<string> expected = {"def","y","(","int","q",")","->","void","{","}"};
    list<string> l = {"def","y","(","int","q",")","->","void","{","/*","}",
                             "def","very_long_func","(","int","xxx",")","->","int","{","if","(","0",">","xxx",")","{","return","xxx","}","*/","}"};

    remove_comments(&l);


    auto it_res = l.begin();
    auto it_exp = expected.begin();

    EXPECT_EQ(l.size(), expected.size());
    for (int i = 0; i<expected.size(); i++){
        ASSERT_EQ(*it_exp,*it_res);
        it_res++;
        it_exp++;
    }
}

TEST_F(LexerTest, comments_within_comments1) {
    list<string> expected = {"def","y","(","int","q",")","->","void","{","}"};
    list<string> l = {"def","y","(","int","q",")","->","void","{","/*","}",
                      "def","very_long_func","(","int","xxx",")","->","int","{","if","/*","(","0",">","*/","xxx",")","{","return","xxx","}","*/","}"};

    remove_comments(&l);


    auto it_res = l.begin();
    auto it_exp = expected.begin();

    EXPECT_EQ(l.size(), expected.size());
    for (int i = 0; i<expected.size(); i++){
        ASSERT_EQ(*it_exp,*it_res);
        it_res++;
        it_exp++;
    }
}
