//
// Created by ray on 01.12.22.
//
#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <vector>
#include "FileEditor.hpp"

class FileEditorTest : public :: testing::Test {
protected:

    void SetUp() override {
        fe = FileEditor();
        fe.content = "def showDigit(int a) -> void {\n"
                   "    putChar(a+48);\n"
                   "}";
    }

    void TearDown() override {}

    FileEditor fe;

};



TEST_F(FileEditorTest, t1) {
    std::string solution = "def printDigit(int a) -> void {\n"
    "    putChar(a+48);\n"
    "}";

    fe.enqueue_change({4,8,"print"});

    fe.apply_changes();

    EXPECT_EQ(fe.content,solution);

}

TEST_F(FileEditorTest, t2) {
    std::string solution = "def (int a) -> void /% _O(1) %/ {\n"
                           "    putChar(a+48);\n"
                           "}";

    fe.enqueue_change({4,13,""});
    fe.enqueue_change({28, 28," /% _O(1) %/"});

    fe.apply_changes();

    EXPECT_EQ(fe.content,solution);

}