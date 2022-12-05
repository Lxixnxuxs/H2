//
// Created by ray on 01.12.22.
//
#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <vector>
#include "Tokenstream.hpp"

class TokenstreamTest : public :: testing::Test {
protected:

    void SetUp() override {

    }

    void TearDown() override {}


};

string toString(Tokenstream& t){
    string res = "";
    auto old_begin = t.begin_;
    while(!t.empty()) {
        res +=  "|";
        res += *t;
        t+=1;
    }
    t.begin_ = old_begin;
    return res;
}


TEST_F(TokenstreamTest, t1) {
    list<string> list = {"(","1","2",")","3","4"};

    auto ts = Tokenstream(&list);
    auto t2 = ts.read_inside_brackets();
    EXPECT_EQ(toString(t2),"|1|2");
    EXPECT_EQ(toString(ts),"|3|4");
}
/*
TEST_F(TokenstreamTest, read_inside_brackets_exception) {
    list<string> list = {"(","1","2","(",")","3","4"};

    auto ts = Tokenstream(&list);

    EXPECT_THROW({
                     try {
                         auto t2 = ts.read_inside_brackets();
                     }
                     catch(std::invalid_argument e){
                         EXPECT_STREQ( "'read_inside_brackets' was not able to find corresponding closing bracket of kind: ')'", e.what());
                     }
                 },std::invalid_argument);
}

TEST_F(TokenstreamTest, read_inside_on_non_bracket) {
    list<string> list = {"1","2","(",")","3","4"};

    auto ts = Tokenstream(&list);

    EXPECT_THROW({
                     try {
                         ts.read_inside_brackets();
                     }
                     catch(std::invalid_argument e){
                         EXPECT_STREQ( "'read_inside_brackets' called on non-bracket token: '1'", e.what());
                     }
                 },std::invalid_argument);
}
*/
TEST_F(TokenstreamTest, t2_) {
    list<string> list = {"(","1","(","2",")",")","3","4"};

    auto ts = Tokenstream(&list);
    auto t2 = ts.read_inside_brackets();
    EXPECT_EQ(toString(t2),"|1|(|2|)");
    EXPECT_EQ(toString(ts),"|3|4");
}

TEST_F(TokenstreamTest, t3) {
    list<string> list = {"{","}","123"};

    auto ts = Tokenstream(&list);
    auto t2 = ts.read_inside_brackets();
    EXPECT_EQ(toString(t2),"");
    EXPECT_EQ(toString(ts),"|123");
}

TEST_F(TokenstreamTest, t4) {
    list<string> list = {"1","2",";","3","45"};

    auto ts = Tokenstream(&list);
    auto t2 = ts.read_until(";");
    EXPECT_EQ(toString(t2),"|1|2");
    EXPECT_EQ(toString(ts),"|3|45");
}

TEST_F(TokenstreamTest, t5) {
    list<string> list = {"=","3","45"};

    auto ts = Tokenstream(&list);
    auto t2 = ts.read_until("=");
    EXPECT_EQ(toString(t2),"");
    EXPECT_EQ(toString(ts),"|3|45");
}

TEST_F(TokenstreamTest, t6) {
    list<string> list = {"=","3","<","4","[","5"};

    auto ts = Tokenstream(&list);
    auto t2 = ts.read_until_one_of({"<","[",">"});
    EXPECT_EQ(toString(t2),"|=|3");
    EXPECT_EQ(toString(ts),"|4|[|5");

}

TEST_F(TokenstreamTest, test_size) {
    list<string> list = {"=","3","<","4","[","5"};

    auto ts = Tokenstream(&list);
    EXPECT_EQ(ts.size(),6);
    EXPECT_EQ(*ts,"=");

}

TEST_F(TokenstreamTest, test_size_edge) {
    list<string> list = {};

    auto ts = Tokenstream(&list);
    EXPECT_EQ(ts.size(),0);

}

TEST_F(TokenstreamTest, read_until_no_instance) {
    list<string> list = {"=","3","<","4","[","5"};

    auto ts = Tokenstream(&list);
    auto t2 = ts.read_until("def");
    EXPECT_EQ(toString(t2),"|=|3|<|4|[|5");
    EXPECT_EQ(toString(ts),"");
    EXPECT_EQ(ts.empty(),true);

}

TEST_F(TokenstreamTest, read_until_one_of_no_instance) {
    list<string> list = {"=","3","<","4","[","5"};

    auto ts = Tokenstream(&list);
    auto t2 = ts.read_until_one_of({"def","main",";"});
    EXPECT_EQ(toString(t2),"|=|3|<|4|[|5");
    EXPECT_EQ(toString(ts),"");
    EXPECT_EQ(ts.empty(),true);

}