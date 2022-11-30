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


    void compile_statement(std::string statement, ParsedFunction& main_f, Accumulator& acc){
        //cout << statement << endl;

        // return statement
        if (statement.find("return") != std::string::npos) {
            Expression e = *Parser::parse_expression(
                    Parser::s_substring(statement, 6, statement.size()), main_f, 0, "%rax");

            acc.push(Expression::compile(e), false);
            acc.push("jmp End");
            // declaration statement
        } else if (statement.find("int") != std::string::npos) {
            auto type = Parser::read_until(statement, ' ');
            auto var_name = Parser::read_until(statement, ' ');
            Parser::read_until(statement, '=');

            Parser::add_variable(main_f, var_name, type);

            Expression e = *Parser::parse_expression(statement, main_f, 0, "%rax");
            acc.push(Expression::compile(e), false);

            acc.push("movq %rax, -" + std::to_string(main_f.var_to_offset[var_name]) + "(%rsp)"); //*

            // assignment
        } else {
            auto var_name = Parser::read_until(statement, ' ');
            Parser::read_until(statement, '=');

            Expression e = *Parser::parse_expression(statement, main_f, 0, "%rax");
            acc.push(Expression::compile(e), false);

            acc.push("movq %rax, -" + std::to_string(main_f.var_to_offset[var_name]) + "(%rsp)");
        }
    }

    void compile_subspace(std::string body, ParsedFunction& main_f, Accumulator& acc){

        //cout << "\n>\n"<< body << "\n<\n"<< endl;
        while (!body.empty()){

            // checking for control structures
            if (body.rfind("if", 0) == 0) {
                Parser::read_until(body);
                auto condition = Parser::read_corresponding_bracket(body,0);
                Expression* e = Parser::parse_expression(condition,main_f, 0 , "%rax");
                Expression* e2 = new Expression(NULL,NULL,LIT,"%r10",0,0);
                Expression top = Expression(e,e2,ADD,"%rax",0,0);
                acc.push(Expression::compile(top,false), false);
                delete e2;


                acc.push("jnz P1" );       // go into if body if expression evaluates to zero

                auto inner = Parser::read_corresponding_bracket(body,0,true);

                compile_subspace(inner,main_f, acc);
                acc.push("jmp A1");     // TOD: How to make dynamic jumppoints for each if statement?
                acc.push("P1:" );

            } else if (body.rfind("else", 0) == 0) {
                Parser::read_until(body);
                auto inner = Parser::read_corresponding_bracket(body,0,true);
                compile_subspace(inner,main_f, acc);
                acc.push("A1:");
                // not a control structure
            } else {
                auto statement = Parser::read_until(body,';');
                compile_statement(statement,main_f,acc);
            }
        }
    }

    std::string compile() {
        std::vector<ParsedFunction> functions = Parser::split_by_functions(file_content);
        ParsedFunction main_f = functions[0];
        //for (auto a : main_f.statements) cout << a << "\n| ";

        Parser::parse_arguments(main_f);
        Accumulator acc;

        acc.set_bookmark("dec_stackframe");


        std::string body = main_f.body;
        compile_subspace(body,main_f,acc);


        size_t stack_frame_size = main_f.current_offset;  //*
        acc.insert_at("dec_stackframe","addq $"+std::to_string(stack_frame_size) + ", %rsp");
        acc.push("End:");
        acc.push("subq $"+std::to_string(stack_frame_size) + ", %rsp");
        acc.push("ret");

        return acc.code;
    }
};
#endif //H2_PARSERMAIN_HPP
