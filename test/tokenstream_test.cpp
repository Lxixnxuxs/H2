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

TEST_F(TokenstreamTest, t1) {
    list<string> list = {"(","1","2",")","3","4"};

    auto ts = Tokenstream(&list);
    auto t2 = ts.read_inside_brackets();
    EXPECT_EQ(toString(t2),"|1|2");
    EXPECT_EQ(toString(ts),"|3|4");
}

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