#ifndef H2_PARSER_HPP
#define H2_PARSER_HPP

#include "Tokenstream.hpp"

#include "abstract_syntax_tree/ASTTree.hpp"

#include <vector>
#include "old/ParsedFunction.hpp"
#include "old/parser.hpp"
#include "LocalVariableManager.hpp"

static std::string keywords[] = {"def", "return", "if", "else"};
static std::string regs[] = {"%rcx", "%r8", "%r9", "%r10", "%r11"};

static std::map<std::string, ComputationOp> op_string_to_type = {{"+", ADD}, {"-", SUB}, {"*", MUL}};

class CodeParser {
private:
    bool is_valid_identifier(std::string token) {
        return keywords->find(token) == -1 && data_types->find(token) == -1 && !std::isdigit(token.at(0));
    }

public:
    size_t global_id_counter = 0;

    ASTRootNode* parse(Tokenstream t) {

        std::vector<ASTFunctionNode*> funcs;

        t+=1; // throw away first def
        while (!t.empty()){
            auto t2 = t.read_until("def");
            funcs.push_back(parse_function(t2));
        }

        return new ASTRootNode(funcs);
    }

    ASTFunctionNode* parse_function(Tokenstream t) {
        // Node that the 'def' has already been thrown away

        LocalVariableManager var_manager;
        string func_name = *t;
        var_manager.name = func_name;
        t += 1;
        auto argument_list = t.read_inside_brackets();
        t += 1; //discard '->'

        var_manager.ret_type = *t;

        t += 1; // discard return type

        auto body = t.read_inside_brackets();

        // TODO parse_argument_list?
        auto parsed_body = parse_subspace(body, var_manager);

        size_t stack_frame_size = var_manager.current_offset;

        return new ASTFunctionNode(func_name, parsed_body, stack_frame_size);

    }

    std::vector<ASTStatementNode*> parse_subspace(Tokenstream t, LocalVariableManager& v){
        std::vector<ASTStatementNode*> res;
        while (!t.empty()){

            // checking for control structures
            if (*t == "if") {
                t += 1;
                auto cond_stream = t.read_inside_brackets();
                auto if_body_stream = t.read_inside_brackets();
                t += 1;

                auto else_body_stream = t.read_inside_brackets();

                res.push_back(parse_if_else(cond_stream, if_body_stream, else_body_stream, v));
            } else {
                // not a control structure
                res.push_back(parse_line(t.read_until(";"), v));
            }
        }

        return res;
    }

    ASTIfElseNode* parse_if_else(Tokenstream condition, Tokenstream if_body, Tokenstream else_body, LocalVariableManager& v) {
        // Parse condition
        ASTCalculationNode* condition_node = parse_calculation(condition, v);
        std::vector<ASTStatementNode*> if_body_nodes = parse_subspace(if_body, v);
        std::vector<ASTStatementNode*> else_body_nodes = parse_subspace(else_body, v);

        return new ASTIfElseNode(condition_node, if_body_nodes, else_body_nodes, global_id_counter++);
    }

    ASTStatementNode* parse_line(Tokenstream t, LocalVariableManager& v){
        // find out if this is a declaration
        if (*t == "return") {
            t += 1;
            ASTCalculationNode* calc = parse_calculation(t, v, 0);
            return new ASTReturnNode(calc, v.name);
        }

        for (auto type : data_types){
            if (type == *t){
                // declare new variable
                t+=1;
                v.add_variable(*t,type);
                break;
            }
        }

        auto var = *t;
        t+=2; // jump over '='
        auto calculation = parse_calculation(t,v);

        return new ASTAssignmentNode(v.var_to_offset[var], calculation);
    }

    ASTCalculationNode* parse_calculation(Tokenstream t, LocalVariableManager& v, int h = 0){
        ASTCalculationNode *left, *right;

        if (t.size() == 1) {
            return parse_literal(*t, v, h);
        }

        // Process left side
        if (*t == "(") {
            Tokenstream left_stream = t.read_inside_brackets();
            left = parse_calculation(left_stream, v, h);
        } else {
            left = parse_literal(*t, v, h);
            t += 1;
        }

        std::string op = *t;
        t += 1;

        // Process right side
        if (*t == "(") {
            Tokenstream right_stream = t.read_inside_brackets();
            right = parse_calculation(right_stream, v, h + 1);
        } else {
            right = parse_literal(*t, v, h + 1);
        }

        return new ASTCalculationNode(left, right, op_string_to_type[op], regs[h]);
    }

    ASTCalculationNode* parse_literal(std::string lit, LocalVariableManager& v, int h) {
        for (auto pair : v.var_to_offset) {
            if (pair.first == lit) {
                // found a variable-definition
                return new ASTCalculationNode(nullptr, nullptr, VAR,regs[h],0,v.var_to_offset[lit]);
            }
        }

        int value;
        try {
            value = std::stoi(lit);
        } catch(int e){
            throw("Variable '" + lit + "' not defined!");
        }

        return new ASTCalculationNode(nullptr, nullptr, LIT, regs[h],value, 0);
    }
};

#endif //H2_PARSER_HPP