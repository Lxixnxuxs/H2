#ifndef H2_PARSER_HPP
#define H2_PARSER_HPP

#include <vector>
#include "Tokenstream.hpp"
#include "LocalVariableManager.hpp"
#include "GlobalVariableManager.hpp"
#include "complexity_analyzer/virtual_math_term.hpp"
#include "abstract_syntax_tree/ASTVariableNode.hpp"

//#include "abstract_syntax_tree/ASTTree.hpp"
struct ASTRootNode;
struct ASTStatementNode;
struct ASTIfElseNode;
struct ASTWhileLoopNode;
struct ASTCallNode;
struct ASTComparisonNode;
struct ASTCalculationNode;
struct ASTCommentNode;
struct ASTClassNode;


class CodeParser {
private:
    string is_valid_identifier(std::string token);

    bool is_valid_data_type(std::string token, GlobalVariableManager& g);

    void expect(Tokenstream t, string token);

    void expect_one_of(Tokenstream t, vector<string> tokens);

    void expect_identifier(Tokenstream t);

    void expect_empty(Tokenstream t);

    void expect_data_type(Tokenstream t, GlobalVariableManager& g);

public:
    size_t global_id_counter = 0;

    std::shared_ptr<ASTRootNode> parse(Tokenstream t);

    std::shared_ptr<ASTClassNode> parse_class(Tokenstream& t, GlobalVariableManager& g);

    std::shared_ptr<ASTFunctionNode> parse_function(Tokenstream& t, GlobalVariableManager& g, std::optional<std::string> class_name = {});

    std::pair<int,vector<std::pair<string,string>>> parse_argument_list(Tokenstream t, LocalVariableManager& v, GlobalVariableManager& g);

    std::vector<std::shared_ptr<ASTStatementNode>> parse_subspace(Tokenstream t, LocalVariableManager& v, GlobalVariableManager& g);

    std::shared_ptr<ASTIfElseNode> parse_if_else(Tokenstream condition, Tokenstream if_body, Tokenstream else_body, LocalVariableManager& v, GlobalVariableManager& g);

    std::shared_ptr<ASTWhileLoopNode> parse_while(Tokenstream condition,Tokenstream complexity_stream, Tokenstream body, LocalVariableManager& v, GlobalVariableManager& g);

    std::map<string, VirtualMathTerm> parse_complexity(Tokenstream t);

    VirtualMathTerm parse_complexity_term(Tokenstream t);

    std::shared_ptr<ASTStatementNode> parse_line(Tokenstream t, LocalVariableManager& v, GlobalVariableManager& g);

    std::shared_ptr<ASTCommentNode> parse_comment(Tokenstream t);

    std::shared_ptr<ASTCallNode> parse_call(Tokenstream& t, LocalVariableManager& v, GlobalVariableManager& g, int h=0);

    std::shared_ptr<ASTComparisonNode> parse_comparison(Tokenstream t, LocalVariableManager& v, GlobalVariableManager& g);

    std::shared_ptr<ASTCalculationNode> parse_calculation(Tokenstream t, LocalVariableManager& v, GlobalVariableManager& g, int h = 0);

    std::shared_ptr<ASTCalculationNode> parse_literal(Tokenstream t, LocalVariableManager &v, GlobalVariableManager &g, int h);

    std::shared_ptr<ASTVariableNode> parse_class_variable(Tokenstream t, bool is_root, std::string reg, LocalVariableManager& local_vars,
                                                          GlobalVariableManager& global_vars, std::string prev_class_name="");
};

#endif //H2_PARSER_HPP