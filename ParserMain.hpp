#ifndef H2_PARSERMAIN_HPP
#define H2_PARSERMAIN_HPP

#include "ParsedFunction.hpp"
#include "Expression.hpp"
#include "Accumulator.hpp"
#include <map>

struct ParserMain {
    std::string file_content;

    ParserMain(std::string filename) {
        std::ifstream f(filename);
        std::stringstream buffer;

        buffer << f.rdbuf();
        file_content = buffer.str();
    }

    std::string compile() {
        std::vector<ParsedFunction> functions = Parser::split_by_functions(file_content);
        ParsedFunction main_f = functions[0];
        //Parser::parse_arguments(main_f);
        Accumulator acc;

        acc.set_bookmark("dec_stackframe");

        for (std::string statement : main_f.statements) {

            // return statement
            if (statement.find("return") != std::string::npos) {
                Expression e = *Parser::parse_expression(
                        Parser::s_substring(statement, 6, statement.size()), 0, "%rax");

                acc.push(Expression::compile(e));
                acc.push("store {expr reg} {var addr + rsp}"); //*

            // declaration statement
            } else if (statement.find("int") != std::string::npos) {
                auto type = Parser::read_until(statement, ' ');
                auto var_name = Parser::read_until(statement, ' ');
                Parser::read_until(statement, '=');

                Parser::add_variable(main_f,var_name,type);

                Expression e = *Parser::parse_expression(statement);
                acc.push(Expression::compile(e));

                acc.push("mov DWROD %rsp ["+std::to_string(main_f.var_to_offset[var_name])+"], {expr reg}"); //*

                // assignment
            } else {
                auto var_name = Parser::read_until(statement, ' ');
                Parser::read_until(statement, '=');

                Expression e = *Parser::parse_expression(statement);
                acc.push(Expression::compile(e));

                acc.push("mov DWROD %rsp ["+std::to_string(main_f.var_to_offset[var_name])+"], {expr reg}"); //*
            }
        }

        size_t stack_frame_size = main_f.current_offset;  //*
        acc.insert_at("dec_stackframe","subi #rsp $"+std::to_string(stack_frame_size));
        acc.push("addi #rsp $"+std::to_string(stack_frame_size));

        return acc.code;
    }


};
#endif //H2_PARSERMAIN_HPP
