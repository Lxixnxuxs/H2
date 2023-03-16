//
// Created by ray on 20.01.23.
//

#include <string>
#include <vector>
#include <map>
/*
// for compiling
const std::vector<std::string> callee_save_regs = {"%r12d","%r13d","%r14d","%r15d","%ecx", "%r10d", "%r11d"};
const std::vector<std::string> argument_regs = {"%edi","%esi","edx","esx","r8d","r9d"};

// all the callee-save regs are saved and can therefore be used
const std::vector<std::string> regs = {"%r12d","%r13d","%r14d","%r15d","%ecx", "%r10d", "%r11d"};

const std::vector<std::string> keywords = {"def", "return", "if", "else", "while"};
const std::vector<std::string> operator_symbols = {"-","+","*","/","%"};
const std::vector<std::string> complexity_operator_symbols = {"+","*","^"};
const std::vector<std::string> comparison_symbols = {"==","!=","<=",">=","<",">"};

const std::vector<std::string> data_types = {"void","int"};



// for analyzing
const int block_indentation = 4;*/
std::map<std::string, size_t> type_to_size = { {"void",0},{"int", 8} };

std::string class_method_name(std::string class_,  std::string func_) {
    return class_ + "__" + func_;
}
