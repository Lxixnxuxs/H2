#ifndef H2_PARSERMAIN_HPP
#define H2_PARSERMAIN_HPP

#include "ParsedFunction.hpp"
#include "Expression.hpp"
#include "Accumulator.hpp"
#include <map>

struct ParserMain {

std::map<std::string, size_t> type_to_size = { {"int", 4} };

struct ParserMain {



    ParserMain(std::string filename) {
        std::ifstream f(filename);
        std::stringstream buffer;

        buffer << f.rdbuf();
        file_content = buffer.str();
    }


    std::string read_until(std::string& s, char c, bool stripped = true) {
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
            auto type = Parser::read_until(text);
            auto name = Parser::read_until(text);

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
                Expression e = *Parser::parse_expression(
                        Parser::s_substring(statement, 6, statement.size()), 0, "%rax");

                acc.push(Expression::compile(e));
                acc.push("   store {expr reg} {var addr + rsp}"); //*

            // declaration statement
            } else if (statement.find("int") != std::string::npos) {
                auto type = Parser::read_until(statement, ' ');
                auto var_name = Parser::read_until(statement, ' ');
                Parser::read_until(statement, '=');

                add_variable(main_f,var_name,type);

                Expression e = *Parser::parse_expression(statement);
                acc.push(Expression::compile(e));


                acc.push("store {expr reg} {"+std::to_string(main_f.var_to_offset[var_name])+" + %rsp}"); //*

                // assignment
            } else {
                auto var_name = Parser::read_until(statement, ' ');
                Parser::read_until(statement, '=');

                Expression e = *Parser::parse_expression(statement);
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
