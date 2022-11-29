#ifndef H2_PARSERMAIN_HPP
#define H2_PARSERMAIN_HPP

#include "ParsedFunction.hpp"
#include "Expression.hpp"
#include "Accumulator.hpp"

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
                auto t = Parser::read_until(statement, ' ');
                auto var_name = Parser::read_until(statement, ' ');
                Parser::read_until(statement, '=');

                main_f.local_v.push_back({var_name, t});

                Expression e = *Parser::parse_expression(statement);
                acc.push(Expression::compile(e));

                acc.push("   store {expr reg} {var addr + rsp}"); //*

                // assignment
            } else {
                auto var_name = Parser::read_until(statement, ' ');
                Parser::read_until(statement, '=');

                Expression e = *Parser::parse_expression(statement);
                acc.push(Expression::compile(e));

                acc.push("   store {expr reg} {var addr + rsp}"); //*
            }
        }

        size_t stack_frame_size = main_f.local_v.size()*4;  //*
        acc.insert_at("dec_stackframe","   subi #rsp $"+std::to_string(stack_frame_size));
        acc.push("   addi #rsp $"+std::to_string(stack_frame_size));

        return acc.code;
    }


};
#endif //H2_PARSERMAIN_HPP
