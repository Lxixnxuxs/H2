//
// Created by ray on 08.12.22.
//



#ifndef H2_GLOBAL_INFORMATION_HPP
#define H2_GLOBAL_INFORMATION_HPP

#include <string>
#include <vector>
#include <map>
/*
extern const std::vector<std::string> callee_save_regs;
extern const std::vector<std::string> argument_regs;

extern const std::vector<std::string> regs;

extern const std::vector<std::string> keywords;
extern const std::vector<std::string> operator_symbols;
extern const std::vector<std::string> complexity_operator_symbols;
extern const std::vector<std::string> comparison_symbols;

extern const std::vector<std::string> data_types;
extern std::map<std::string, size_t> type_to_size;

extern const int block_indentation;
*/
// for compiling
const std::vector<std::string> callee_save_regs = {"%r12d","%r13d","%r14d","%r15d","%ecx", "%r10d", "%r11d"};
const std::vector<std::string> argument_regs = {"%edi","%esi","edx","esx","r8d","r9d", "rcx"};

// all the callee-save regs are saved and can therefore be used
const std::vector<std::string> regs = {"%r12d","%r13d","%r14d","%r15d", "%r10d", "%r11d"};

const std::vector<std::string> keywords = {"def", "return", "if", "else", "while"};
const std::vector<std::string> operator_symbols = {"-","+","*","/","%","&","|","€","<<",">>"};
const std::vector<std::string> complexity_operator_symbols = {"+","*","^"};
const std::vector<std::string> comparison_symbols = {"==","!=","<=",">=","<",">"};

const std::vector<std::string> data_types = {"void","int"};
extern std::map<std::string, size_t> type_to_size; // = { {"void",0},{"int", 4} }

const int block_indentation = 4;
#endif //H2_GLOBAL_INFORMATION_HPP
