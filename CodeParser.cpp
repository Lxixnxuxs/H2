
#include <stdexcept>
#include <algorithm>
#include "Tokenstream.hpp"
#include "abstract_syntax_tree/ASTTree.hpp"
#include "lexer.hpp"
#include "global_information.hpp"

#include "CodeParser.hpp"

static std::map<std::string, ComputationOp> op_string_to_type = {{"+", ADD}, {"-", SUB}, {"*", MUL}, {"/", DIV},
                                                                 {"%", MOD}};


    string CodeParser::is_valid_identifier(std::string token) {
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

    bool CodeParser::is_valid_data_type(std::string token, GlobalVariableManager& g) {
        return (g.class_exists(token) || find(data_types.begin(),data_types.end(),token) != data_types.end());
    }

    void CodeParser::expect(Tokenstream t, string token){
        if (*t != token){
            throw std::invalid_argument("PARSER ERROR  Expected '"+token+"' but recieved '"+*t+"'");
        }
    }

    void CodeParser::expect_one_of(Tokenstream t, vector<string> tokens){
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

    void CodeParser::expect_identifier(Tokenstream t) {
        string message = is_valid_identifier(*t);
        if (!message.empty()){
            throw std::invalid_argument("PARSER ERROR  "+message);
        }
    }

    void CodeParser::expect_empty(Tokenstream t) {
        if (!t.empty()){
            auto t_copy = t;
            string end_message = "";
            for (int i = 0; i<std::min((int)t.size(),5); i++) {
                end_message += " '"+*t_copy+"'";
                t_copy+=1;
            }
            throw std::invalid_argument("PARSER ERROR  Expected an empty stream but received a stream of size "+std::to_string(t.size())+" starting with " + end_message );
        }


    }

    void CodeParser::expect_data_type(Tokenstream t, GlobalVariableManager &g) {
        if (!is_valid_data_type(*t,g)){
            throw std::invalid_argument("PARSER ERROR  Expected a data type but received '"+*t+"'");
        }
    }

    ASTRootNode* CodeParser::parse(Tokenstream t) {
        if (t.empty()) {
            throw std::invalid_argument("PARSER ERROR  cannot parse empty programm");
        }
        GlobalVariableManager g;
        std::vector<ASTNode*> funcs_and_classes;


        /*
        while (!t.empty()){
            if (token_found == "def") {
                auto res = t.read_until_one_of({"def","class"});
                auto t2 = res.first;
                token_found = res.second;
                //if (!t2.empty()) { // I think this is not needed
                funcs_and_classes.push_back(parse_function(t2, g));

            } else if (token_found == "class"){ // class definition
                auto res = t.read_until_one_of({"def","class"});
                auto t2 = res.first;
                token_found = res.second;
                //if (!t2.empty()) { // I think this is not needed
                funcs_and_classes.push_back(parse_class(t2, g));
            } else {
                throw std::invalid_argument("PARSER ERROR  Expected 'def' or 'class' but received '"+*t+"'");
            }
        }
         */

        while (!t.empty()) {
            expect_one_of(t,{"def","class"});
            std::string token_found = *t;
            t+=1; // throw away first def or class

            if (token_found == "def") {
                funcs_and_classes.push_back(parse_function(t, g));      // ATTENTION: at this spot the tokenstream needs to be passed by value!
            } else if (token_found == "class") {
                funcs_and_classes.push_back(parse_class(t, g));
            }
        }

        return new ASTRootNode(funcs_and_classes);
    }

    ASTClassNode* CodeParser::parse_class(Tokenstream& t, GlobalVariableManager& g){
        // ATTENTION: the tokenstream is passed by reference!
        // Note that the 'class' has already been thrown away

        expect_identifier(t);
        string class_name = *t;
        t+=1; // throw away class_name

        // find parameters
        expect(t,"[");
        auto parameter_stream = t.read_inside_brackets();
        std::vector<std::string> parameters;

        while(!parameter_stream.empty()) {
            expect_identifier(parameter_stream);
            parameters.push_back(*parameter_stream);
            parameter_stream+=1; // throw away parameter

            if (!parameter_stream.empty()) {
                expect(parameter_stream,",");
                parameter_stream+=1; // throw away ','
            }
        }

        // body
        expect(t,"{");
        auto class_content = t.read_inside_brackets();
        //expect_empty(t);

        // class variables
        LocalVariableManager class_intern_offset_manager;

        auto class_var_stream = class_content.read_until("def");

        GlobalVariableManager useless_g;
        while (!class_var_stream.empty()) {
            parse_line(class_var_stream.read_until(";"), class_intern_offset_manager, useless_g); // maybe write own function instead. Otherwise wrong programm will be accepted
        }


        std::vector<std::shared_ptr<ASTFunctionNode>> class_functions;

        // class is defined before the Functions are parsed
        g.class_to_local_manager[class_name] = class_intern_offset_manager;

        // functions
        while (!class_content.empty()) {
            //expect(class_content,"def");
            //std::string token_found = *class_content;
            //class_content+=1; // throw away first def or class
            auto next_function = class_content.read_until("def");
            class_functions.push_back((std::shared_ptr<ASTFunctionNode>) parse_function(next_function, g, {class_name}));

            /*
            // effect on parameter
            expect(next_function, "[");
            next_function+=1;
            // TODO parse parameter changes
            expect(next_function, "]");
            next_function+=1;*/
            expect_empty(next_function);
        }

        return new ASTClassNode(class_name, parameters, class_intern_offset_manager, class_functions);
    }


ASTFunctionNode* CodeParser::parse_function(Tokenstream& t, GlobalVariableManager& g,  std::optional<std::string> class_name) {
        // ATTENTION: the tokenstream is passed by reference!
        // Note that the 'def' has already been thrown away



        LocalVariableManager var_manager;

        // add implicit 'this' argument (reference)
        if (class_name) var_manager.add_variable("this",class_name.value(),&g, true);

        string func_name = *t;
        var_manager.name = func_name;

        auto* res = new ASTFunctionNode();
        g.var_to_node[func_name] = res; // Node Object is known to global variable manager
        res->f_name = func_name;

        expect_identifier(t);
        t += 1; // discard func_name

        expect(t,"(");
        auto argument_stream = t.read_inside_brackets();

        std::pair<int,vector<std::pair<string,string>>> temp  = parse_argument_list(argument_stream, var_manager, g);

        res->arg_stackpart_size = temp.first;

        res->argument_list = temp.second;

        g.var_to_argument_list[func_name] = res->argument_list;

        expect(t,"->");
        t += 1; //discard '->'

        var_manager.ret_type = *t;

        expect_data_type(t, g);
        res->return_type = *t;

        t += 1; // discard return type

        map<string, VirtualMathTerm> complexity_map;
        if (*t == "/%") {
            // a complexity annotation is given
            Tokenstream complexity_stream = t.read_inside_brackets();
            res->initialize_with_complexity_map(parse_complexity(complexity_stream));
        }


        expect(t,"{");
        auto body = t.read_inside_brackets();


        auto parsed_body = parse_subspace(body, var_manager, g);
        res->body = parsed_body;

        // expect nothing to be there after closing bracket '}'
        //expect_empty(t);

        size_t stack_frame_size = var_manager.current_offset;
        res->f_stack_size = stack_frame_size;


        //auto res = new ASTFunctionNode(func_name, parsed_body, stack_frame_size, arg_stack_size,arg_list, return_type, complexity_map);
        //g.var_to_node[func_name] = res; // Node Object is known to global variable manager
        return res;
    }

    std::pair<int,vector<std::pair<string,string>>> CodeParser::parse_argument_list(Tokenstream t, LocalVariableManager& v, GlobalVariableManager& g) {
        // returns args_stack_size

        vector<std::pair<string,string>> type_list;
        bool is_first_iteration = true;

        while (!t.empty()) {
            if (is_first_iteration) {
                is_first_iteration = false;
            } else {
                expect(t,",");
                t+=1; // discard ',' between arguments
            }
            expect_data_type(t, g);

            string type = *t;
            t+=1; // discard type

            expect_identifier(t);
            string name = *t;
            t+=1; // discard name

            v.add_variable(name, type, &g,type!="int"); // everything except primitive types are passed by reference
            type_list.emplace_back(name, type);
        }
        return {v.current_offset, type_list};
    }

    std::vector<ASTStatementNode*> CodeParser::parse_subspace(Tokenstream t, LocalVariableManager& v, GlobalVariableManager& g){
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
                Tokenstream complexity_stream; // TODO does this work with empty complexity?
                if (*t == "/%") {
                    complexity_stream = t.read_inside_brackets();
                }
                expect(t,"{");
                auto body_stream = t.read_inside_brackets();
                res.push_back(parse_while(cond_stream,complexity_stream,body_stream,v,g));

            } else if (*t == "/*") {
                // a comment
                res.push_back(parse_comment(t.read_inside_brackets()));

            } else {
                // not a control structure
                res.push_back(parse_line(t.read_until(";"), v,g));
            }
        }

        return res;
    }

    ASTIfElseNode* CodeParser::parse_if_else(Tokenstream condition, Tokenstream if_body, Tokenstream else_body, LocalVariableManager& v, GlobalVariableManager& g) {
        // Parse condition
        ASTComparisonNode* condition_node = parse_comparison(condition, v, g);
        std::vector<ASTStatementNode*> if_body_nodes = parse_subspace(if_body, v, g);
        std::vector<ASTStatementNode*> else_body_nodes = parse_subspace(else_body, v, g);

        return new ASTIfElseNode(condition_node, if_body_nodes, else_body_nodes, global_id_counter++);
    }

    ASTWhileLoopNode* CodeParser::parse_while(Tokenstream condition,Tokenstream complexity_stream, Tokenstream body, LocalVariableManager& v, GlobalVariableManager& g) {
        ASTComparisonNode* condition_node = parse_comparison(condition, v, g);
        auto complexity_map = parse_complexity(complexity_stream);
        std::vector<ASTStatementNode*> body_nodes = parse_subspace(body, v, g);

        return new ASTWhileLoopNode(condition_node,body_nodes,global_id_counter++, complexity_map);
    }

    std::map<string, VirtualMathTerm> CodeParser::parse_complexity(Tokenstream t) {
        // returns a map from "O" or "I" to a VirtualMathTerm
        // Only custom terms are regarded!
        std::map<string, VirtualMathTerm> res;

        while (!t.empty()) {

            expect_one_of(t,{"O","I","_O","_I","C","_C"});
            std::string op = *t;
            bool custom = (op[0] != '_');
            t+=1; // disregard operator

            expect(t,"(");
            Tokenstream term_stream = t.read_inside_brackets();
            VirtualMathTerm term = parse_complexity_term(term_stream);
            if (custom) res[op] = term;

        }

        return res;
    }

    VirtualMathTerm CodeParser::parse_complexity_term(Tokenstream t) {
        // TODO implement
        if (t.empty()) {
            throw std::invalid_argument("PARSER ERROR  cannot parse empty complexity_term");
        }

        // variable or number (checked in the constructor)
        if (t.size() == 1) return {*t};

        // take negative numbers into account
        if (t.size() == 2 and *t == "-") {
            t+=1; // disregard '-'
            try {
                double nr = stoi(*t);
                return {-1*nr};
            } catch (...) {
                throw std::invalid_argument("PARSER ERROR  cannot parse '-[non_number]' ");
            }
        }

        if (*t=="log") {
            t+=1; // disregard log
            expect(t,"(");
            Tokenstream arg_stream = t.read_inside_brackets();

            expect_empty(t); // should be empty, because log (without brackets) can only occur without any + * etc. concatenation

            auto first_arg = arg_stream.read_until(",");
            if (arg_stream.empty()) {   // only one argument -> implicit log10(arg)
                return VirtualMathTerm(LOGARITHM, {10,parse_complexity_term(first_arg)});
            } else {
                return VirtualMathTerm(LOGARITHM, {parse_complexity_term(first_arg),parse_complexity_term(arg_stream)});
            }
        }

        VirtualMathTerm first;
        VirtualMathTerm second;
        string operation;

        // first part of the operation
        if (*t == "(") {
            Tokenstream t1 = t.read_inside_brackets();
            first = parse_complexity_term(t1);
            expect_one_of(t,complexity_operator_symbols);
            operation = *t;

        } else {
            first = VirtualMathTerm(*t);
            t+=1;  // disregard first part
            expect_one_of(t,complexity_operator_symbols);
            operation = *t;

        }

        // second part of the operation
        t+=1; // disregard operation
        if (t.size()==1){
            second = parse_complexity_term(t);
            t+=1; // disregard second part
        }
        else {
            expect(t,"(");
            second = parse_complexity_term(t.read_inside_brackets());
        }

        expect_empty(t); // there should be nothing behind the second part of the operation. Otherwise, use brackets!

        // putting both together
        VirtualMathTerm res;
        if (operation == "+") res = VirtualMathTerm(ADDITION);
        if (operation == "*") res = VirtualMathTerm(MULTIPLICATION);
        if (operation == "^") res = VirtualMathTerm(EXPONENTIAL);

        res.children.push_back(first);
        res.children.push_back(second);

        return res;
    }

    ASTStatementNode* CodeParser::parse_line(Tokenstream t, LocalVariableManager& v, GlobalVariableManager& g){

        if (*t == "return") {
            t += 1; // discard 'return'
            ASTCalculationNode* calc = parse_calculation(t, v, g, 0);
            return new ASTReturnNode(calc, v.name);
        }



        bool need_to_declare = false;
        string type_;

        // find out if this is a declaration
        // declare new variable later
        if (is_valid_data_type(*t, g)){
            need_to_declare = true;
            type_ = *t;
            t+=1; // discard type
        }

        // check for expression without '='. eg a funcion call or something like an unused '3+4;'
        // all this is handled in parse_calculation
        auto copy = t;
        copy+=1;
        if (*copy != "=" && copy.size()!=0) {
            return parse_calculation(t,v,g);
        }


        expect_identifier(t);
        if (!need_to_declare) {
            // if it is not a declaration, check if the variable exists
            if (!v.variable_exists(*t) and !v.get_this_namespace(g).variable_exists(*t)) {
                throw std::invalid_argument("PARSER ERROR  variable '" + *t + "' not declared");
            }
        }
        auto var = *t;
        t+=1; // discard var_name

        // declaration without assignment
        if (t.empty() && need_to_declare){
            v.add_variable(var,type_, &g, true); // Obj a;  this creates a new object
            return new ASTAssignmentNode(v.var_to_offset[var], nullptr, var,type_,need_to_declare);
        }

        expect(t,"=");
        t+=1; // discard '='

        // expect the whole remaining tokens to belong to the calculation
        auto calculation = parse_calculation(t,v,g);



        // declare variable only after the calculation, because the parser needs to check that this very variable is not
        // used within its own declaration
        if (need_to_declare){
            v.add_variable(var,type_, &g, false);  // Obj b = a; where a is of type Obj. This only copies the reference
        }

        // TODO introduce typ checking

        return new ASTAssignmentNode(v.var_to_offset[var], calculation, var,type_,need_to_declare);
    }





    ASTCallNode* CodeParser::parse_call(Tokenstream& t, LocalVariableManager& v, GlobalVariableManager& g, int h){
        // be aware that this call changes the Tokenstream of the higher level

        expect_identifier(t);
        string func_name = *t;
        vector<string> expected_types;
        bool defined = false;
        for (const auto pair : g.var_to_argument_list) {
            //cout << "defined: "+ pair.first+" "<< pair.second.size() << endl;
            if (pair.first == func_name){
                defined = true;
                expected_types;
                for (const auto p : pair.second) {
                    expected_types.push_back(p.first);
                }
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

        return new ASTCallNode(nullptr, nullptr,VAR,"",0,0,g.var_to_node[func_name],arguments,h);

    }

    ASTComparisonNode* CodeParser::parse_comparison(Tokenstream t, LocalVariableManager& v, GlobalVariableManager& g){
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

    ASTCalculationNode* CodeParser::parse_calculation(Tokenstream t, LocalVariableManager& v, GlobalVariableManager& g, int h){
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

    ASTCalculationNode* CodeParser::parse_literal(std::string lit, LocalVariableManager& v, GlobalVariableManager& g, int h) {

        if (v.variable_exists(lit)) {
            return new ASTCalculationNode(nullptr, nullptr, VAR,regs[h],0,v.var_to_offset[lit], lit);
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

ASTCommentNode *CodeParser::parse_comment(Tokenstream t) {
    return new ASTCommentNode(
            t.to_string()
            );
}


