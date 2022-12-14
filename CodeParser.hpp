#ifndef H2_PARSER_HPP
#define H2_PARSER_HPP

#include <stdexcept>
#include <algorithm>
#include "Tokenstream.hpp"

#include "abstract_syntax_tree/ASTTree.hpp"

#include <vector>
#include "old/ParsedFunction.hpp"
#include "old/parser.hpp"
#include "LocalVariableManager.hpp"
#include "GlobalVariableManager.hpp"
#include "lexer.hpp"
#include "global_information.hpp"


static std::map<std::string, ComputationOp> op_string_to_type = {{"+", ADD}, {"-", SUB}, {"*", MUL}, {"/", DIV},
                                                                 {"%", MOD}};

class CodeParser {
private:
    string is_valid_identifier(std::string token) {
        // returns a error message. If it returns empty string, the identifier is valid

        if (find(keywords.begin(),keywords.end(),token) != keywords.end()) {
            return "'"+token+"' cannot be used as a identifier, because it is a keyword";
        }
        if (find(data_types.begin(),data_types.end(),token) != data_types.end()){
            return "'"+token+"' cannot be used as a identifier, because it specifies datatype";
        }
        if (std::isdigit(token.at(0))) {
            return "'"+token+"' cannot be used as a identifier, because it starts with a number";
        }
        if (find(lexer_symbols.begin(), lexer_symbols.end(),token) != lexer_symbols.end()){
            return "'"+token+"' cannot be used as a identifier, because it is a lexer symbol";
        }
        return "";
    }

    void expect(Tokenstream t, string token){
        if (*t != token){
            throw std::invalid_argument("PARSER ERROR  Expected '"+token+"' but recieved '"+*t+"'");
        }
    }

    void expect_one_of(Tokenstream t, vector<string> tokens){
        bool valid = false;
        for (int i = 0; i<tokens.size(); i++){
            if (*t == tokens[i]){
                valid = true;
            }
        }
        if (!valid){
            string message = "PARSER ERROR  Expected one of: ";
            for (int i = 0; i<tokens.size(); i++){
                message+= "'"+tokens[i] + "' ";
            }
            message += " but recieved '"+*t+"'";
            throw std::invalid_argument(message);
        }
    }

    void expect_identifier(Tokenstream t) {
        string message = is_valid_identifier(*t);
        if (!message.empty()){
            throw std::invalid_argument("PARSER ERROR  "+message);
        }
    }

    void expect_empty(Tokenstream t) {
        if (!t.empty()){
            auto t_copy = t;
            string end_message = "";
            for (int i = 0; i<std::min((int)t.size(),5); i++) {
                end_message += " '"+*t_copy+"'";
                t_copy+=1;
            }
            throw std::invalid_argument("PARSER ERROR  Expected an empty stream but recieved a stream of size "+std::to_string(t.size())+" starting with " + end_message );
        }


    }

public:
    size_t global_id_counter = 0;

    ASTRootNode* parse(Tokenstream t) {
        if (t.empty()) {
            throw std::invalid_argument("PARSER ERROR  cannot parse empty programm");
        }
        GlobalVariableManager g;
        std::vector<ASTFunctionNode*> funcs;

        expect(t,"def");
        t+=1; // throw away first def

        while (!t.empty()){
            auto t2 = t.read_until("def");
            if (!t2.empty()){
                funcs.push_back(parse_function(t2, g));
            }

        }

        return new ASTRootNode(funcs);
    }

    ASTFunctionNode* parse_function(Tokenstream t, GlobalVariableManager& g) {
        // Node that the 'def' has already been thrown away

        LocalVariableManager var_manager;
        string func_name = *t;
        var_manager.name = func_name;

        expect_identifier(t);
        t += 1; // discard func_name

        expect(t,"(");
        auto argument_list = t.read_inside_brackets();

        std::pair<int,vector<string>> temp  = parse_argument_list(argument_list, var_manager);
        int arg_stack_size = temp.first;
        vector<string> arg_list = temp.second;

        g.var_to_argument_list[func_name] = arg_list;

        expect(t,"->");
        t += 1; //discard '->'

        var_manager.ret_type = *t;

        expect_one_of(t,data_types);
        t += 1; // discard return type

        expect(t,"{");
        auto body = t.read_inside_brackets();


        auto parsed_body = parse_subspace(body, var_manager, g);

        // expect nothing to be there after closing bracket '}'
        expect_empty(t);

        size_t stack_frame_size = var_manager.current_offset;

        return new ASTFunctionNode(func_name, parsed_body, stack_frame_size, arg_stack_size,arg_list);

    }

    std::pair<int,vector<string>> parse_argument_list(Tokenstream t, LocalVariableManager& v){
        // returns args_stack_size

        vector<string> type_list;

        while (!t.empty()) {
            expect_one_of(t,data_types);
            string type = *t;
            t+=1; // discard type

            expect_identifier(t);
            string name = *t;
            t+=1; // discard name

            v.add_variable(name, type);
            type_list.push_back(type);
        }
        return std::pair<int,vector<string>>{v.current_offset,type_list};
    }

    std::vector<ASTStatementNode*> parse_subspace(Tokenstream t, LocalVariableManager& v, GlobalVariableManager& g){
        std::vector<ASTStatementNode*> res;
        while (!t.empty()){

            // checking for control structures
            if (*t == "if") {
                t += 1; // discard 'if'
                expect(t,"(");
                auto cond_stream = t.read_inside_brackets();
                expect(t,"{");
                auto if_body_stream = t.read_inside_brackets();
                expect(t,"else");
                t += 1; // discard 'else'
                expect(t,"{");
                auto else_body_stream = t.read_inside_brackets();
                res.push_back(parse_if_else(cond_stream, if_body_stream, else_body_stream, v,g));

            } else if (*t == "while"){
                t += 1; // discard 'while'
                expect(t,"(");
                auto cond_stream = t.read_inside_brackets();
                expect(t,"{");
                auto body_stream = t.read_inside_brackets();
                res.push_back(parse_while(cond_stream,body_stream,v,g));

            } else {
                // not a control structure
                res.push_back(parse_line(t.read_until(";"), v,g));
            }
        }

        return res;
    }

    ASTIfElseNode* parse_if_else(Tokenstream condition, Tokenstream if_body, Tokenstream else_body, LocalVariableManager& v, GlobalVariableManager& g) {
        // Parse condition
        ASTComparisonNode* condition_node = parse_comparison(condition, v, g);
        std::vector<ASTStatementNode*> if_body_nodes = parse_subspace(if_body, v, g);
        std::vector<ASTStatementNode*> else_body_nodes = parse_subspace(else_body, v, g);

        return new ASTIfElseNode(condition_node, if_body_nodes, else_body_nodes, global_id_counter++);
    }

    ASTWhileLoopNode* parse_while(Tokenstream condition, Tokenstream body, LocalVariableManager& v, GlobalVariableManager& g) {
        ASTComparisonNode* condition_node = parse_comparison(condition, v, g);
        std::vector<ASTStatementNode*> body_nodes = parse_subspace(body, v, g);

        return new ASTWhileLoopNode(condition_node,body_nodes,global_id_counter++);
    }

    ASTStatementNode* parse_line(Tokenstream t, LocalVariableManager& v, GlobalVariableManager& g){

        if (*t == "return") {
            t += 1; // discard 'return'
            ASTCalculationNode* calc = parse_calculation(t, v, g, 0);
            return new ASTReturnNode(calc, v.name);
        }



        bool need_to_declare = false;
        string type_;

        // find out if this is a declaration
        // declare new variable later
        for (auto type : data_types){

            if (type == *t){
                need_to_declare = true;
                type_ = type;
                t+=1; // discard type
                break;
            }
        }

        // check for expression without '='. eg a funcion call or something like an unused '3+4;'
        // all this is handled in parse_calculation
        auto copy = t;
        copy+=1;
        if (*copy != "=") {
            return parse_calculation(t,v,g);
        }


        expect_identifier(t);
        auto var = *t;
        t+=1; // discard var_name

        expect(t,"=");
        t+=1; // discard '='

        // expect the whole remaining tokens to belong to the calculation
        auto calculation = parse_calculation(t,v,g);



        // declare variable only after the calculation, because the parser needs to check that this very variable is not
        // used within its own declaration
        if (need_to_declare){
            v.add_variable(var,type_);
        }

        return new ASTAssignmentNode(v.var_to_offset[var], calculation);
    }

    ASTCallNode* parse_call(Tokenstream& t, LocalVariableManager& v, GlobalVariableManager& g, int h=0){
        // be aware that this call changes the Tokenstream of the higher level

        expect_identifier(t);
        string func_name = *t;
        vector<string> expected_types;
        bool defined = false;
        for (const auto pair : g.var_to_argument_list) {
            //cout << "defined: "+ pair.first+" "<< pair.second.size() << endl;
            if (pair.first == func_name){
                defined = true;
                expected_types = pair.second;
            }
            //cout << defined << endl;
        }
        //cout << endl;

        if (!defined) {
            throw std::invalid_argument("PARSER ERROR  function '"+func_name+"' not defined");
        }
        t+=1; // discard func_name
        expect(t, "[");
        auto arguments_stream = t.read_inside_brackets();

        vector<ASTCalculationNode*> arguments;

        int i = 0;
        while (!arguments_stream.empty()) {
            if (i>=expected_types.size()) throw std::invalid_argument("PARSER ERROR  '"+func_name+
                    "' was called with too many arguments (with more than "+ std::to_string(expected_types.size())+")");

            auto stream = arguments_stream.read_until(",");
            arguments.push_back(parse_calculation(stream,v,g,h+i));
            // here should be implemented a type-check for the argument
            i++;

        }

        if (i<expected_types.size()) throw std::invalid_argument("PARSER ERROR  '"+func_name+
        "' was called with not enought arguments: "+std::to_string(i)+" instead of "+ std::to_string(expected_types.size())+"");

        return new ASTCallNode(nullptr, nullptr,VAR,"",0,0,func_name,arguments,h);

    }

    ASTComparisonNode* parse_comparison(Tokenstream t, LocalVariableManager& v, GlobalVariableManager& g){
        ASTCalculationNode *left, *right;

        if (t.empty()){
            throw std::invalid_argument("PARSER ERROR: trying to parse an empty comparison");
        }

        if (t.size() == 1) {
            throw std::invalid_argument("PARSER ERROR: trying to parse an comparison, but only recieved '"+*t+"'");
        }

        // Process left side
        if (*t == "(") {
            Tokenstream left_stream = t.read_inside_brackets();
            if (t.empty()) {
                // there are top-level brackets around the comparison. Look inside
                return parse_comparison(left_stream,v,g);
            }
            left = parse_calculation(left_stream, v,g, 0);
        } else {
            left = parse_literal(*t, v,g, 0);
            t += 1; // discard literal
        }

        std::string op = *t;

        expect_one_of(t,comparison_symbols);
        t += 1; // discard operator symbol

        // Process right side
        if (*t == "(") {
            Tokenstream right_stream = t.read_inside_brackets();
            right = parse_calculation(right_stream, v,g,  1);
        } else {
            right = parse_literal(*t, v,g,  1);
            t += 1; // discard literal
        }

        expect_empty(t);

        return new ASTComparisonNode(left, right, op, regs[0],regs[1]);
    }

    ASTCalculationNode* parse_calculation(Tokenstream t, LocalVariableManager& v, GlobalVariableManager& g, int h = 0){
        ASTCalculationNode *left, *right;

        if (t.empty()){
            throw std::invalid_argument("PARSER ERROR: trying to parse an empty calculation");
        }

        if (t.size() == 1) {
            return parse_literal(*t, v, g,h);
        }

        // checking for function call
        Tokenstream copy = t;
        copy += 1;
        if (*copy == "[") {
            copy.read_inside_brackets();
            if (copy.empty()) {     // only if it is just a function call and nothing more
                return parse_call(t,v,g,h);
            }
        }

        // Process left side
        if (*t == "(") {
            Tokenstream left_stream = t.read_inside_brackets();
            if (t.empty()) {
                // there are top-level brackets around the calculation. Look inside
                return parse_calculation(left_stream,v,g,h);
            }
            left = parse_calculation(left_stream, v,g, h);
        } else {
            // checking for function call
            Tokenstream copy = t;
            copy += 1;
            if (*copy == "[") {
                left = parse_call(t,v,g,h);
            } else {
                left = parse_literal(*t, v, g, h);
                t += 1; // discard literal
            }
        }

        std::string op = *t;

        expect_one_of(t,operator_symbols);
        t += 1; // discard operator symbol

        // Process right side
        if (*t == "(") {
            Tokenstream right_stream = t.read_inside_brackets();
            right = parse_calculation(right_stream, v,g, h + 1);
        } else {
            // checking for function call
            Tokenstream copy = t;
            copy += 1;
            if (*copy == "[") {
                right = parse_call(t,v,g, h+1);
            } else {
                right = parse_literal(*t, v, g, h + 1);
                t += 1; // discard literal
            }
        }

        expect_empty(t);

        return new ASTCalculationNode(left, right, op_string_to_type[op], regs[h]);
    }

    ASTCalculationNode* parse_literal(std::string lit, LocalVariableManager& v, GlobalVariableManager& g, int h) {
        for (auto pair : v.var_to_offset) {
            if (pair.first == lit) {
                // found a variable-definition
                return new ASTCalculationNode(nullptr, nullptr, VAR,regs[h],0,v.var_to_offset[lit]);
            }
        }

        int value;
        try {
            value = std::stoi(lit);
        } catch(...){
            // first check if this even is a valid identifier
            //list<string> temp = {lit};
            //expect_identifier(Tokenstream(&temp));
            throw std::invalid_argument("PARSER ERROR: Variable '" + lit + "' not defined");
        }

        return new ASTCalculationNode(nullptr, nullptr, LIT, regs[h],value, 0);
    }
};

#endif //H2_PARSER_HPP