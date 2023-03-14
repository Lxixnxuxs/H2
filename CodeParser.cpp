
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include "Tokenstream.hpp"
#include "abstract_syntax_tree/ASTTree.hpp"
#include "lexer.hpp"
#include "global_information.hpp"

#include "CodeParser.hpp"
#include "abstract_syntax_tree/ASTVariableNode.hpp"

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

bool CodeParser::is_valid_data_type(std::string token, std::shared_ptr<GlobalVariableManager> g) {
    return (g->class_exists(token) || find(data_types.begin(),data_types.end(),token) != data_types.end());
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

void CodeParser::expect_data_type(Tokenstream t, std::shared_ptr<GlobalVariableManager> g) {
    if (!is_valid_data_type(*t,g)){
        throw std::invalid_argument("PARSER ERROR  Expected a data type but received '"+*t+"'");
    }
}

std::shared_ptr<ASTRootNode> CodeParser::parse(Tokenstream t) {
        if (t.empty()) {
            throw std::invalid_argument("PARSER ERROR  cannot parse empty programm");
        }
        auto g = std::make_shared<GlobalVariableManager>();


        //declaring default functions, which can be called from any context
        std::string default_print_name = "putChar";
        auto default_print_func = std::make_shared<ASTFunctionNode>();
        default_print_func->f_name = default_print_name;
        default_print_func->argument_list = {{"int","a"}};
        g->var_to_argument_list[default_print_name] = default_print_func->argument_list;
        g->var_to_node[default_print_name] = default_print_func;



        std::vector<std::shared_ptr<ASTNode>> funcs_and_classes;


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

        return std::make_shared<ASTRootNode>(funcs_and_classes);
    }

std::shared_ptr<ASTClassNode> CodeParser::parse_class(Tokenstream& t, std::shared_ptr<GlobalVariableManager> g){
        // ATTENTION: the tokenstream is passed by reference!
        // Note that the 'class' has already been thrown away

    expect_identifier(t);
    string class_name = *t;
    t+=1; // throw away class_name

    // creating dummy entry to show that class exists; will be overwritten after structure is read
    g->class_to_local_manager[class_name] = {};

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
    auto class_intern_offset_manager = std::make_shared<LocalVariableManager>();

    auto class_var_stream = class_content.read_until("def");

    while (!class_var_stream.empty()) {
        parse_class_variable(class_var_stream.read_until(";"), class_intern_offset_manager, g, {class_name});
    }

    std::vector<std::shared_ptr<ASTFunctionNode>> class_functions;

    // class is defined before the Functions are parsed
    g->class_to_local_manager[class_name] = class_intern_offset_manager;

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

        return std::make_shared<ASTClassNode>(class_name, parameters, class_intern_offset_manager, class_functions);
    }

void CodeParser::parse_class_variable(Tokenstream t, std::shared_ptr<LocalVariableManager> class_variable_representation, std::shared_ptr<GlobalVariableManager> g, std::optional<std::string> class_name){
    expect_data_type(t,g);
    std::string data_type = *t;
    t+=1;
    expect_identifier(t);
    std::string var_name = *t;
    t+=1;
    expect_empty(t);
    class_variable_representation->add_variable(var_name, data_type, g, (data_type != "int"));
}


std::shared_ptr<ASTFunctionNode> CodeParser::parse_function(Tokenstream& t, std::shared_ptr<GlobalVariableManager> g,  std::optional<std::string> class_name) {
        // ATTENTION: the tokenstream is passed by reference!
        // Note that the 'def' has already been thrown away

    auto var_manager = std::make_shared<LocalVariableManager>();


    string func_name = *t;
    var_manager->name = func_name;

        auto res = std::make_shared<ASTFunctionNode>();
        g->var_to_node[func_name] = res; // Node Object is known to global variable manager
        res->f_name = func_name;

    expect_identifier(t);
    t += 1; // discard func_name

    expect(t,"(");
    auto argument_stream = t.read_inside_brackets();

    std::pair<int,vector<std::pair<string,string>>> temp  = parse_argument_list(argument_stream, var_manager, g, class_name);

    res->arg_stackpart_size = temp.first;

    res->argument_list = temp.second;

    g->var_to_argument_list[func_name] = res->argument_list;

    expect(t,"->");
    t += 1; //discard '->'

    var_manager->ret_type = *t;

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


    auto parsed_body = parse_subspace(body, var_manager, g, class_name);
    res->body = parsed_body;

    // expect nothing to be there after closing bracket '}'
    //expect_empty(t);

    res->stack_frame = var_manager;


    //auto res = new ASTFunctionNode(func_name, parsed_body, stack_frame_size, arg_stack_size,arg_list, return_type, complexity_map);
    //g.var_to_node[func_name] = res; // Node Object is known to global variable manager
    return res;
}

std::pair<int,vector<std::pair<string,string>>> CodeParser::parse_argument_list(Tokenstream t, std::shared_ptr<LocalVariableManager> v, std::shared_ptr<GlobalVariableManager> g,  std::optional<std::string> class_name) {
    // returns args_stack_size



    vector<std::pair<string,string>> type_list;

    auto addVar = [&](std::string name, std::string type){
        v->add_variable(name, type, g,type!="int"); // everything except primitive types are passed by reference
        type_list.emplace_back(name, type);
    };

    bool is_first_iteration = true;

    // if class function, define 'this' variable for all purposes
    if (class_name) addVar("this",class_name.value());

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

        addVar(name,type);
        }
        return {v->current_offset, type_list};
    }

    std::vector<std::shared_ptr<ASTStatementNode>> CodeParser::parse_subspace(Tokenstream t, std::shared_ptr<LocalVariableManager> v, std::shared_ptr<GlobalVariableManager> g, std::optional<std::string> class_name){
        std::vector<std::shared_ptr<ASTStatementNode>> res;
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
            res.push_back(parse_if_else(cond_stream, if_body_stream, else_body_stream, v,g,class_name));

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
            res.push_back(parse_while(cond_stream,complexity_stream,body_stream,v,g,class_name));

        } else if (*t == "/*") {
            // a comment
            res.push_back(parse_comment(t.read_inside_brackets()));

        } else {
            // not a control structure
            res.push_back(parse_line(t.read_until(";"), v,g, class_name));
        }
    }

    return res;
}

std::shared_ptr<ASTIfElseNode> CodeParser::parse_if_else(Tokenstream condition, Tokenstream if_body, Tokenstream else_body, std::shared_ptr<LocalVariableManager> v, std::shared_ptr<GlobalVariableManager> g, std::optional<std::string> class_name) {
        // Parse condition
    std::shared_ptr<ASTComparisonNode> condition_node = parse_comparison(condition, v, g, class_name);
        std::vector<std::shared_ptr<ASTStatementNode>> if_body_nodes = parse_subspace(if_body, v, g, class_name);
        std::vector<std::shared_ptr<ASTStatementNode>> else_body_nodes = parse_subspace(else_body, v, g, class_name);

        return std::make_shared<ASTIfElseNode>(condition_node, if_body_nodes, else_body_nodes, global_id_counter++);
    }

std::shared_ptr<ASTWhileLoopNode> CodeParser::parse_while(Tokenstream condition,Tokenstream complexity_stream, Tokenstream body, std::shared_ptr<LocalVariableManager> v, std::shared_ptr<GlobalVariableManager> g, std::optional<std::string> class_name) {
    std::shared_ptr<ASTComparisonNode> condition_node = parse_comparison(condition, v, g, class_name);
        auto complexity_map = parse_complexity(complexity_stream);
        std::vector<std::shared_ptr<ASTStatementNode>> body_nodes = parse_subspace(body, v, g, class_name);

        return std::make_shared<ASTWhileLoopNode>(condition_node,body_nodes,global_id_counter++, complexity_map);
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
    // TODO enable parsing of floats with . eg 1.5   will not work now
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

std::shared_ptr<ASTStatementNode> CodeParser::parse_line(Tokenstream t, std::shared_ptr<LocalVariableManager> v,
                                                         std::shared_ptr<GlobalVariableManager> g, std::optional<std::string> class_name){

        if (*t == "return") {
            t += 1; // discard 'return'
            std::shared_ptr<ASTCalculationNode> calc = parse_calculation(t, v, g, 0, class_name);
            auto tmp = new ASTReturnNode(calc, v->name);
            auto res =  std::shared_ptr<ASTReturnNode>(tmp);
            return res;
        }


        std::optional<std::string> declaration_type = (is_valid_data_type(*t, g) ? std::optional<string>(*t) : std::nullopt);
        if (declaration_type) t+=1; // discard data type
        int type_size = (declaration_type) ? g->get_type_size(declaration_type.value()) : 0;


        auto copy_stream = t;
        copy_stream.read_until("=");
        bool is_assignment = !copy_stream.empty();


        // calcualtion without target, e.g.:  'f(x);' '3+4;'
        if (!is_assignment && !declaration_type) {
            return parse_calculation(t,v,g,0,class_name);
        }

        // class variable access only consists of '.' and valid identifiers
        auto target_variable_stream = t.read_while([this](string s){return (s=="." or is_valid_identifier(s).empty());});
        auto var = parse_variable(target_variable_stream, true, regs[0], v, g, class_name, (bool) declaration_type);

        // new pure declaration, e.g.: 'int x;'
        if (!is_assignment && declaration_type) {
            /*// new declared variables cannot be inside classes, therefore only one token expected
            expect_identifier(t);
            auto var = *t;
            t+=1; // discard var_name*/

            expect_empty(t); // there should be nothing behind the variable name
            v->add_variable(var->name,declaration_type.value(), g, true);   // new declared variables cannot be inside classes, therefore only one token expected

            // converts per default into 'int x = 0;' // TODO:  will this cause trouble?
            return std::make_shared<ASTAssignmentNode>(0, std::nullopt,
                    var, declaration_type.value(), true, type_size);
        }


        // the following cases are left: 'x = 3;' or 'x = 3;'
            expect(t,"=");
            t+=1;

            std::shared_ptr<ASTCalculationNode> calc = parse_calculation(t, v, g, 1, class_name);
            // important that the calculation starts with register 1 (not 0),
            // because register one is needed to evaluate a class variable in 'classvariable = calculation'

            if (declaration_type) v->add_variable(var->name,declaration_type.value(), g, true);   // new declared variables cannot be inside classes, therefore only one token expected
            return std::make_shared<ASTAssignmentNode>(0, calc, var, declaration_type.value_or(""), (bool) declaration_type, type_size);




        /*



    //expect_identifier(t);
        if (!need_to_declare) {
            // if it is not a declaration, check if the variable exists
            auto target_variable_stream = t.read_while([this](string s){return (s=="." or is_valid_identifier(s).empty());});
            auto variable_node = parse_variable(target_variable_stream,true,regs[0],v,g,class_name);


            //if (!v->variable_exists(*t) and !v->get_this_namespace(g)->variable_exists(*t)) {
            //    throw std::invalid_argument("PARSER ERROR  variable '" + *t + "' not declared");
            //}
        } else {
            // new declared variables cannot be inside classes, therefore only one token expected
            var = *t;
            t+=1; // discard var_name

            // declaration without assignment
            if (t.empty()){
                v->add_variable(var,type_, g, true); // Obj a;  this creates a new object
                return std::make_shared<ASTAssignmentNode>(v->var_to_offset[var], nullptr, var,type_,need_to_declare);
            }
        }

        expect(t,"=");
        t+=1; // discard '='

        // expect the whole remaining tokens to belong to the calculation
        auto calculation = parse_calculation(t,v,g);



        // declare variable only after the calculation, because the parser needs to check that this very variable is not
        // used within its own declaration
        if (need_to_declare){
            v->add_variable(var,type_, g, false);  // Obj b = a; where a is of type Obj. This only copies the reference
        }

        // TODO introduce typ checking

        return std::make_shared<ASTAssignmentNode>(v->var_to_offset[var], calculation, var,type_,need_to_declare);
        */
    }





std::shared_ptr<ASTCallNode> CodeParser::parse_call(Tokenstream& t, std::shared_ptr<LocalVariableManager> v,
                                                    std::shared_ptr<GlobalVariableManager> g, int h, std::optional<std::string> class_name){
        // be aware that this call changes the Tokenstream of the higher level


        auto var_stream = t.read_while([this](std::string token){return (token=="." or is_valid_identifier(token).empty());});
        if (var_stream.empty()) throw std::invalid_argument("PARSER ERROR: trying to parse function without name");

        string func_name;
        std::optional<Tokenstream> implicit_argument_stream = {};

        // in case it is not just a normal call f(), but something like a.b.f()
        if (var_stream.size() != 1) {
            auto copy = var_stream;
            while (copy.size() > 2) copy += 1;
            implicit_argument_stream = Tokenstream(var_stream.begin_, copy.begin_);
            expect(copy,".");
            copy+=1;
            expect_identifier(copy);
            func_name = *copy;
        } else {func_name = *var_stream;}


        if (!g->function_exists(func_name)) {
            throw std::invalid_argument("PARSER ERROR: trying to call function '" + func_name + "', which does not exist");
        }
        auto argument_list = g->var_to_argument_list[func_name];


        expect(t, "(");
        auto arguments_stream = t.read_inside_brackets();

        vector<std::shared_ptr<ASTCalculationNode>> arguments;



        // check number of arguments in advance
        auto arguments_stream_copy = arguments_stream;
        int argument_count = 0;

    if (implicit_argument_stream) { // optionally adding implicit argument
        arguments.push_back(parse_calculation(implicit_argument_stream.value(),v,g,h + arguments.size(), class_name));
        argument_count++;
    }

        while (!arguments_stream_copy.empty()) {
            argument_count++;
            arguments_stream_copy.read_until(",");
        }

        if (argument_count == argument_list.size() - 1 and !implicit_argument_stream and class_name) {
            // there may be one argument missing, because 'f()' instead of 'this.f()' was written
            std::list<std::string> l = {"this"};
            Tokenstream tmp = &l;

            auto temp = parse_calculation(tmp,v,g,h, class_name);
            arguments.insert(arguments.begin(), temp);
        }

        while (!arguments_stream.empty()) {
            if (arguments.size()>=argument_list.size()) throw std::invalid_argument("PARSER ERROR  '"+func_name+
                          "' was called with too many arguments (with more than "+ std::to_string(argument_list.size())+")");

            auto stream = arguments_stream.read_until(",");
            arguments.push_back(parse_calculation(stream,v,g,h + arguments.size(), class_name));
            // here should be implemented a type-check for the argument
        }

        if (arguments.size()<argument_list.size()) throw std::invalid_argument("PARSER ERROR  '"+func_name+
                           "' was called with not enough arguments: "+std::to_string(arguments.size())+" instead of "+ std::to_string(argument_list.size())+"");

        return std::make_shared<ASTCallNode>(nullptr, nullptr,VAR,"",0,0,g->var_to_node[func_name],arguments,h);

    }



std::shared_ptr<ASTComparisonNode> CodeParser::parse_comparison(Tokenstream t, std::shared_ptr<LocalVariableManager> v,
                                                                std::shared_ptr<GlobalVariableManager> g, std::optional<std::string> class_name){
    std::shared_ptr<ASTCalculationNode> left, right;

        if (t.empty()){
            throw std::invalid_argument("PARSER ERROR: trying to parse an empty comparison");
        }

        if (t.size() == 1) {
            throw std::invalid_argument("PARSER ERROR: trying to parse an comparison, but only recieved '"+*t+"'");
        }

        auto first_calculation_stream = t.read_until_one_of(comparison_symbols, true);
        auto operation = *t;
        t+=1;

        left = parse_calculation(first_calculation_stream,v,g,0, class_name);
        right = parse_calculation(t,v,g,1, class_name);

        return std::make_shared<ASTComparisonNode>(left, right, operation, regs[0],regs[1]);

    }

std::shared_ptr<ASTCalculationNode> CodeParser::parse_calculation(Tokenstream t, std::shared_ptr<LocalVariableManager> v,
                                                                  std::shared_ptr<GlobalVariableManager> g, int h, std::optional<std::string> class_name) {
    std::string most_recent_operation = "";
    std::optional<std::shared_ptr<ASTCalculationNode>> calculation_until_now = {};
    bool next_comes_operator = false;

    // function to safely build the calculation tree
    auto add_to_calculation = [&](std::shared_ptr<ASTCalculationNode> node){
        if (!calculation_until_now) {
            calculation_until_now = node;
        } else {
            calculation_until_now = std::make_shared<ASTCalculationNode>(calculation_until_now.value(), node, op_string_to_type[most_recent_operation], regs[h]);
        }
    };

    if (t.empty()) {
        throw std::invalid_argument("PARSER ERROR: trying to parse an empty calculation");
    }

    while(!t.empty()) {
        if (!next_comes_operator) {
            if (*t == "(") {
                add_to_calculation(parse_calculation(t.read_inside_brackets(), v, g, h, class_name));
                h+=1; // reuse all but one register from sub-calculation
            } else {
                add_to_calculation(parse_literal(t, v, g, h, class_name));
                h+=1; // go to next calculation register
            }
        } else {
            expect_one_of(t, operator_symbols);
            most_recent_operation = *t;
            t+=1; // discard operator
        }
        next_comes_operator = !next_comes_operator;
    }

    return calculation_until_now.value();


}

    std::shared_ptr<ASTCalculationNode> CodeParser::parse_literal(Tokenstream& t, std::shared_ptr<LocalVariableManager> v,
                                                                  std::shared_ptr<GlobalVariableManager> g, int h, std::optional<std::string> class_name) {
    // receives open stream and extracts one symbol from it
    if (t.empty()) {
        throw std::invalid_argument("PARSER ERROR: trying to parse an empty literal");
    }

    // try to interpret as a number
    try {
        int value = std::stoi(*t);  // this may cause an error, it it isn't a number
        t+=1; // discard number
        return std::make_shared<ASTCalculationNode>(nullptr, nullptr, LIT, regs[h], value, 0);
    } catch (...) {}


    auto copy = t;
    copy.read_while([this](std::string token){return (token=="." or is_valid_identifier(token).empty());});
    if (!copy.empty() && *copy == "(") {
        // has syntax of function call, because between identifier and bracket is no operator
        return parse_call(t,v,g,h, class_name);
    }

    // interpret as variable
    // read until next operator. All until it must belong to the variable

        auto res = parse_variable(t.read_while(
                                          [this](std::string token) { return (token == "." or is_valid_identifier(token).empty()); }), true,
                                  regs[h], v, g, class_name);
    return res;

    }

std::shared_ptr<ASTCommentNode> CodeParser::parse_comment(Tokenstream t) {
    return std::make_shared<ASTCommentNode>(
            t.to_string()
            );
}

std::shared_ptr<ASTVariableNode> CodeParser::parse_variable(Tokenstream t, bool is_root, std::string reg,
                                                            std::shared_ptr<LocalVariableManager> local_vars,
                                                            std::shared_ptr<GlobalVariableManager> global_vars,
                                                            std::optional<std::string> prev_class_name, bool is_declaration, bool comes_after_implicit_this) {



    if (!is_root && !comes_after_implicit_this) {
        expect(t, ".");
        t+=1; // discard dot
    }

    if(prev_class_name and !global_vars->class_exists(prev_class_name.value())) {
        throw std::invalid_argument("PARSER ERROR: Class " + prev_class_name.value() + " does not exist");
    }

    // check if variable is defined
    if (is_root) {
        if (!(is_declaration && t.size()==1 /*last level may be undefined for declarations*/) && !local_vars->variable_exists(*t)) {

            // maybe it is a class variable ( 'attr' instead of 'this.attr')
            // this late check makes sure that local variables will shadow class variable
            if (prev_class_name and global_vars->class_to_local_manager[prev_class_name.value()]->variable_exists(*t)) {
                // start this function again at same point, but know that a 'this' is inserted above
                auto child = parse_variable(t, false, reg, local_vars, global_vars, prev_class_name, is_declaration,
                                            true);
                return std::make_shared<ASTVariableNode>("this", child, is_root, local_vars, global_vars, reg);

            } else {
                throw std::invalid_argument("PARSER ERROR: Variable '" + *t + "' not defined");
            }
        }
    } else {
        if (!global_vars->class_to_local_manager[prev_class_name.value()]->variable_exists(*t)) {
            throw std::invalid_argument("PARSER ERROR: Class " + prev_class_name.value() + " has no member " + *t);
        }
    }


    if (t.size() == 1) {
        // base case
        return std::shared_ptr<ASTVariableNode>(std::make_shared<ASTVariableNode>(*t, nullptr, is_root, local_vars, global_vars, reg));
    } else {
        std::string instance_name = *t;
        t += 1;

        std::string class_name;
        if (is_root) {
            // extract class out of local variables
            class_name = local_vars->var_to_type[instance_name];
        } else {
            // extract class out of parent class structure
            class_name = global_vars->class_to_local_manager[prev_class_name.value()]->var_to_type[instance_name];
        }

        std::shared_ptr<ASTVariableNode> child = parse_variable(t, false, reg, local_vars, global_vars, class_name);

        return std::make_shared<ASTVariableNode>(instance_name, child, is_root, local_vars, global_vars, reg);
    }
}