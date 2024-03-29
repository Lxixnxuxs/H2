#ifndef H2_PARSER_HPP
#define H2_PARSER_HPP

#include <vector>
#include "Tokenstream.hpp"
#include "LocalVariableManager.hpp"
#include "GlobalVariableManager.hpp"
#include "virtual_math_term.hpp"

//#include "abstract_syntax_tree/ASTTree.hpp"
struct ASTRootNode;
struct ASTStatementNode;
struct ASTIfElseNode;
struct ASTWhileLoopNode;
struct ASTCallNode;
struct ASTComparisonNode;
struct ASTCalculationNode;


class CodeParser {
private:
    string is_valid_identifier(std::string token);

    void expect(Tokenstream t, string token);

    void expect_one_of(Tokenstream t, vector<string> tokens);

    void expect_identifier(Tokenstream t);

    void expect_empty(Tokenstream t);

public:
    size_t global_id_counter = 0;

    ASTRootNode* parse(Tokenstream t);

    ASTFunctionNode* parse_function(Tokenstream t, GlobalVariableManager& g);

    std::pair<int,vector<std::pair<string,string>>> parse_argument_list(Tokenstream t, LocalVariableManager& v);

    std::vector<ASTStatementNode*> parse_subspace(Tokenstream t, LocalVariableManager& v, GlobalVariableManager& g);

    ASTIfElseNode* parse_if_else(Tokenstream condition, Tokenstream if_body, Tokenstream else_body, LocalVariableManager& v, GlobalVariableManager& g);

    ASTWhileLoopNode* parse_while(Tokenstream condition,Tokenstream complexity_stream, Tokenstream body, LocalVariableManager& v, GlobalVariableManager& g);

    std::map<string, VirtualMathTerm> parse_complexity(Tokenstream t);

    VirtualMathTerm parse_complexity_term(Tokenstream t);

    ASTStatementNode* parse_line(Tokenstream t, LocalVariableManager& v, GlobalVariableManager& g);

    ASTCallNode* parse_call(Tokenstream& t, LocalVariableManager& v, GlobalVariableManager& g, int h=0);

    ASTComparisonNode* parse_comparison(Tokenstream t, LocalVariableManager& v, GlobalVariableManager& g);

    ASTCalculationNode* parse_calculation(Tokenstream t, LocalVariableManager& v, GlobalVariableManager& g, int h = 0);

    ASTCalculationNode* parse_literal(std::string lit, LocalVariableManager& v, GlobalVariableManager& g, int h);
};

#endif //H2_PARSER_HPP