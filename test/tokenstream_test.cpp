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

TEST_F(TokenstreamTest, t1) {
    list<string> list = {"def", "main","(",")","->","void","{",
                             "int","x","=","10",";",
                             "x","=","(","2",")","*","(","x",")",";",
                             "if","(","(","x",")","-","(","20",")",")","{",
                             "return","10",";",
                             "}","else","{",
                             "return","20",";","}",
                             "}"};
    auto ts = Tokenstream(&list);
    auto it = ts.begin_;


}

TEST_F(TokenstreamTest, t2) {

}
