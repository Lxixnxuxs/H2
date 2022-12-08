//
// Created by ray on 08.12.22.
//

#include <string>
#include <vector>

#ifndef H2_GLOBAL_INFORMATION_HPP
#define H2_GLOBAL_INFORMATION_HPP




const std::vector<std::string> callee_save_regs = {"%r12","%r13","%r14","%r15"};
const std::vector<std::string> argument_regs = {"%rdi","%rsi","rdx","rsx","r8","r9"};

// all the callee-save regs are saved and can therefore be used
const std::vector<std::string> regs = {"%r12","%r13","%r14","%r15","%rcx", "%r10", "%r11"};

const std::vector<std::string> keywords = {"def", "return", "if", "else", "while"};
const std::vector<std::string> operator_symbols = {"-","+","*","/"};
const std::vector<std::string> comparison_symbols = {"==","!=","<=",">=","<",">"};

static vector<std::string> data_types = {"void","int"};
static std::map<std::string, size_t> type_to_size = { {"void",0},{"int", 4} };

#endif //H2_GLOBAL_INFORMATION_HPP
