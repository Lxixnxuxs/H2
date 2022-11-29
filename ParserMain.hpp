#ifndef H2_PARSERMAIN_HPP
#define H2_PARSERMAIN_HPP

#include "ParsedFunction.hpp"
#include "Expression.hpp"
#include "Accumulator.hpp"
#include <map>

std::string regs[] = {"%rax","%rcx", "%r8", "%r9", "%r10", "%r11"};
std::string arg_regs[] = {"%rdi","%rsi","%rdx"};
char operators[] = {'+', '-', '*', '/'};

std::map<std::string, size_t> type_to_size = { {"int", 4} };


class ParserMain {
    Parser* parser;

public:
    ParserMain(std::string filename) {
        parser = new Parser(filename);
    }

    size_t pos_matching_bracket(std::string s, size_t i, bool isCurly = false) {
        char b = isCurly ? '{' : '(';
        char c = isCurly ? '}' : ')';

        size_t counter = 0, j = i;

        while (j < s.size()) {
            if (s.at(j) == c && counter == 0) {
                break;
            } else if (s.at(j) == c && counter > 0) {
                counter--;
            } else if (s.at(j) == b) {
                counter++;
            }

            j++;
        }

        return j < s.size() ? j : -1;
    }

    std::string substring_between_brackets(std::string s, size_t i, bool isCurly = false) {
        return s_substring(s, i + 1, pos_matching_bracket(s, i + 1));
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

    Expression* parse_expression(std::string s, int i = 0, std::string r = "") {
        if (find_operator(s) == std::string::npos) {
            // base case
            Expression* e = new Expression(nullptr, nullptr, Operation::LIT, regs[i], std::stoi(s));
            return e;
        } else {
            // recursive call
            std::string left_operand = substring_between_brackets(Parser::trim(s), 0);

            size_t op_pos = find_operator(s, left_operand.size() + 2);
            char op = s.at(op_pos);

            //char op = s.at(left_operand.size() + 2);
            std::string right_operand = substring_between_brackets(s, s.find('(', op_pos));

            Expression* e;

            if (r.empty()) {
                r = regs[i];
            }

            if (op == '+') {
                e = new Expression(parse_expression(left_operand, i), parse_expression(right_operand, i + 1),
                                   Operation::ADD, r, 0);
            } else if (op == '-') {
                e = new Expression(parse_expression(left_operand, i), parse_expression(right_operand, i + 1),
                                   Operation::SUB, r, 0);
            } else if (op == '*') {
                e = new Expression(parse_expression(left_operand, i), parse_expression(right_operand, i + 1),
                                   Operation::MUL, r, 0);
            } else if (op == '/') {
                e = new Expression(parse_expression(left_operand, i), parse_expression(right_operand, i + 1),
                                   Operation::DIV, r, 0);
            }

            return e;
        }
    }

    std::string s_substring(std::string s, size_t from, size_t to) {
        return s.substr(from, to - from);
    }

    /*
    std::pair<std::string, std::string> read_until(std::string s, char c, bool stripped = true) {
        size_t pos = s.find(c);
        std::string s0 = s.substr(0, pos);
        std::string s1 = s_substring(s, pos + 1, s.size());

        if (stripped) {
            Parser::ltrim(s0);
            Parser::rtrim(s1);
        }

        return {s0, s1};
    }
    */

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

    void add_variable(ParsedFunction p, std::string name, std::string type){
        p.var_to_type[name] = type;
        p.var_to_offset[name] = p.current_offset;
        p.current_offset += type_to_size[type];
    }


    void parse_arguments(ParsedFunction p){
        std::string text = p.arguments;
        while (not Parser::trim(text).empty()) {
            auto type = read_until(text);
            auto name = read_until(text);

            add_variable(p,name,type);
        }
    }


    std::string to_assembly() {
        std::vector<ParsedFunction> functions = parser->split_by_functions();
        ParsedFunction main_f = functions[0];
        parse_arguments(main_f);
        Accumulator acc;

        acc.set_bookmark("dec_stackframe");

        for (std::string statement : main_f.statements) {

            // return statement
            if (statement.find("return") != std::string::npos) {
                Expression e = *parse_expression(s_substring(statement, 6, statement.size()), 0, "%rax");

                acc.push(Expression::compile(e));
                acc.push("   store {expr reg} {var addr + rsp}"); //*

            // declaration statement
            } else if (statement.find("int") != std::string::npos) {
                auto type = read_until(statement, ' ');
                auto var_name = read_until(statement, ' ');
                read_until(statement, '=');

                add_variable(main_f,var_name,type);

                Expression e = *parse_expression(statement);
                acc.push(Expression::compile(e));


                acc.push("store {expr reg} {"+std::to_string(main_f.var_to_offset[var_name])+" + %rsp}"); //*

                // assignment
            } else {
                auto var_name = read_until(statement, ' ');
                read_until(statement, '=');

                Expression e = *parse_expression(statement);
                acc.push(Expression::compile(e));

                acc.push("store {expr reg} {"+std::to_string(main_f.var_to_offset[var_name])+" + %rsp}"); //*
            }
        }

        size_t stack_frame_size = main_f.current_offset;  //*
        acc.insert_at("dec_stackframe","   subi #rsp $"+std::to_string(stack_frame_size));
        acc.push("   addi #rsp $"+std::to_string(stack_frame_size));

        return acc.code;
    }


};
#endif //H2_PARSERMAIN_HPP
