//
// Created by ray on 08.12.22.
//

#include <string>
#include <vector>

#ifndef H2_GLOBAL_INFORMATION_HPP
#define H2_GLOBAL_INFORMATION_HPP


const std::vector<std::string> keywords = {"def", "return", "if", "else", "while"};
const std::vector<std::string> regs = {"%rcx", "%r8", "%r9", "%r10", "%r11"};
const std::vector<std::string> callee_save_regs = {"%r12","%r13","%r14","%r15"};

const std::vector<std::string> operator_symbols = {"-","+","*","/"};
const std::vector<std::string> comparison_symbols = {"==","!=","<=",">=","<",">"};

#endif //H2_GLOBAL_INFORMATION_HPP
