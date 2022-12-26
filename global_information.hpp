//
// Created by ray on 08.12.22.
//

#include <string>
#include <vector>

#ifndef H2_GLOBAL_INFORMATION_HPP
#define H2_GLOBAL_INFORMATION_HPP

const std::vector<std::string> callee_save_regs = {"%r12d","%r13d","%r14d","%r15d","%ecx", "%r10d", "%r11d"};
const std::vector<std::string> argument_regs = {"%edi","%esi","edx","esx","r8d","r9d"};

// all the callee-save regs are saved and can therefore be used
const std::vector<std::string> regs = {"%r12d","%r13d","%r14d","%r15d","%ecx", "%r10d", "%r11d"};

const std::vector<std::string> keywords = {"def", "return", "if", "else", "while"};
const std::vector<std::string> operator_symbols = {"-","+","*","/","%"};
const std::vector<std::string> comparison_symbols = {"==","!=","<=",">=","<",">"};

const vector<std::string> data_types = {"void","int"};
std::map<std::string, size_t> type_to_size = { {"void",0},{"int", 4} };


#endif //H2_GLOBAL_INFORMATION_HPP
