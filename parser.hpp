#ifndef H2_PARSER_HPP
#define H2_PARSER_HPP

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include "ParsedFunction.hpp"
#include "Expression.hpp"

namespace Parser {
    static char operators[] = {'+', '-', '*', '/'};
    static std::string regs[] = {"%rcx", "%r8", "%r9", "%r10", "%r11"};
    static std::map<std::string, size_t> type_to_size = { {"int", 8} };

    // trim from start
    inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                        std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }

    // trim from end
    inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                             std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }

    // trim from both ends
    inline std::string &trim(std::string s) {
        return ltrim(rtrim(s));
    }

    /**
     * Computes the position of the first character present in the next occurrence of @param s
     * starting from position @param i
     */
    size_t find_next_string(std::string f, size_t i, std::string s) {
        return f.find(s, i);
    }

    /**
     * Computes the number of characters between position @param i and the next occurrence of the character @param c
     * starting from i, excluding position i
     */
    size_t num_chars_until(std::string f, size_t i, char c) {
        return f.find(c, i) - i;
    }

    /**
     * Computes the position of the next non-whitespace (newline, whitespace, tab) character starting from @param i
     */
    size_t next_non_whitespace_character(std::string f, size_t i) {
        size_t j = i;
        while (j < f.size() &&
              (f.at(j) == '\n' || f.at(j) == ' ' || f.at(j) == '\r')) {
            j++;
        }
        return j;
    }

    /**
     * Computes the position of the corresponding closing bracket starting from @param i = (position+1) of its opening
     * bracket. If @param isCurly true, matches for curly brackets. If false matches for normal brackets
     */
    size_t pos_matching_bracket(std::string f, size_t i, bool isCurly=false) {
        char b = isCurly ? '{' : '(';
        char c = isCurly ? '}' : ')';

        size_t counter = 0, j = i;

        while (j < f.size()) {
            if (f.at(j) == c && counter == 0) {
                break;
            } else if (f.at(j) == c && counter > 0) {
                counter--;
            } else if (f.at(j) == b) {
                counter++;
            }

            j++;
        }

        return j < f.size() ? j : -1;
    }

    std::vector<std::string> extract_statements(std::string s) {
        size_t pos = 0, pos_end;
        std::vector<std::string> statements;

        pos_end = s.find(';', pos);

        while (pos_end != std::string::npos) {
            statements.push_back(trim(s.substr(pos, pos_end - pos)));

            pos = pos_end + 1;
            pos_end = s.find(';', pos);
        }

        return statements;
    }

    std::string s_substring(std::string s, size_t from, size_t to) {
        return s.substr(from, to - from);
    }

    std::string substring_between_brackets(std::string s, size_t i, bool isCurly = false) {
        return s_substring(s, i + 1, pos_matching_bracket(s, i + 1, isCurly));
    }

    std::string read_corresponding_bracket(std::string& s, size_t i, bool isCurly = false, bool stripped = true) {
        /* mutating the string s and extract all the content within the brackets and cutting it out of s*/

        size_t pos = pos_matching_bracket(s, i + 1,isCurly);
        auto res = s_substring(s, i + 1 , pos);
        s = s_substring(s,pos+1,s.size());

        if (stripped) {
            Parser::ltrim(s);
            res = Parser::trim(res);
        }

        return res;
    }


    size_t find_operator(std::string s, size_t i = 0) {
        for (char op : operators) {
            size_t pos = s.find(op, i);
            if (pos != std::string::npos) {
                return pos;
            }
        }

        return std::string::npos;
    }

    std::string read_until(std::string& s, char c = ' ', bool stripped = true) {
        size_t pos = s.find(c);
        std::string res = s.substr(0, pos);
        s = s_substring(s, pos + 1, s.size());

        if (stripped) {
            Parser::ltrim(s);
            Parser::rtrim(res);
        }

        return res;
    }

    std::vector<ParsedFunction> split_by_functions(std::string file_content) {
        std::vector<ParsedFunction> functions;

        size_t function_pos = find_next_string(file_content, 0, "def");

        while (function_pos != std::string::npos) {
            size_t f_name_pos = next_non_whitespace_character(file_content, function_pos + 3);
            size_t f_name_end_pos = num_chars_until(file_content, f_name_pos, '(');

            size_t f_args_pos = find_next_string(file_content, f_name_pos, "(") + 1;
            size_t f_args_end_pos = pos_matching_bracket(file_content, f_args_pos, false);

            size_t f_type_pos =
                    next_non_whitespace_character(file_content,
                                                  next_non_whitespace_character(file_content, f_args_end_pos + 1) + 2);
            size_t f_type_end_pos = find_next_string(file_content, f_type_pos, " ");

            size_t f_block_pos = next_non_whitespace_character(file_content, f_type_end_pos) + 1;
            size_t f_block_end_pos = pos_matching_bracket(file_content, f_block_pos + 1, true) - 1;

            ParsedFunction f(file_content.substr(f_name_pos, f_name_end_pos),
                             file_content.substr(f_args_pos, f_args_end_pos - f_args_pos),
                             file_content.substr(f_type_pos, f_type_end_pos - f_type_pos),
                             //extract_statements(
                             file_content.substr(f_block_pos, f_block_end_pos - f_block_pos + 1));

            functions.push_back(f);

            function_pos = find_next_string(file_content, function_pos + 1, "def");
        }

        return functions;
    }

    Expression* parse_expression(std::string s, ParsedFunction& func, int i = 0, std::string r = "") {
        if (r.empty()) {
            r = regs[i];
        }

        if (find_operator(s) == std::string::npos) {
            // base case
            std::string s1 = s;
            s1 = Parser::trim(s1);

            for(auto i = func.var_to_offset.begin(); i != func.var_to_offset.end(); i++){
                auto key = (*i).first;
                if (key == s1) {
                    // variable
                    Expression* e = new Expression(nullptr, nullptr, Operation::VAR, r,
                                                   0, -func.var_to_offset[key]);
                    return e;
                }
            }

            Expression* e = new Expression(nullptr, nullptr, Operation::LIT, r, std::stoi(s));
            return e;
        } else {
            // recursive call
            std::string left_operand = substring_between_brackets(trim(s), 0);

            size_t op_pos = find_operator(s, left_operand.size() + 2);
            char op = s.at(op_pos);

            //char op = s.at(left_operand.size() + 2);
            std::string right_operand = substring_between_brackets(s, s.find('(', op_pos));

            Expression* e;

            if (op == '+') {
                e = new Expression(parse_expression(left_operand, func, i), parse_expression(right_operand, func, i + 1),
                                   Operation::ADD, r, 0);
            } else if (op == '-') {
                e = new Expression(parse_expression(left_operand, func, i), parse_expression(right_operand, func, i + 1),
                                   Operation::SUB, r, 0);
            } else if (op == '*') {
                e = new Expression(parse_expression(left_operand, func, i), parse_expression(right_operand, func, i + 1),
                                   Operation::MUL, r, 0);
            } else if (op == '/') {
                e = new Expression(parse_expression(left_operand, func, i), parse_expression(right_operand, func, i + 1),
                                   Operation::DIV, r, 0);
            }

            return e;
        }
    }

    void add_variable(ParsedFunction& p, std::string name, std::string type){
        p.var_to_type[name] = type;

        p.var_to_offset[name] = p.current_offset;       // in which order do these two lines need to be?
        p.current_offset += type_to_size[type];

    }

    void parse_arguments(ParsedFunction& p){
        std::string text = p.arguments;
        while (not Parser::trim(text).empty()) {
            auto type = Parser::read_until(text);
            auto name = Parser::read_until(text);

            add_variable(p,name,type);
        }
    }
};

#endif //H2_PARSER_HPP