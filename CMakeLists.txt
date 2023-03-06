cmake_minimum_required(VERSION 3.22)
project(H2)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(test)
add_subdirectory(abstract_syntax_tree)
include_directories(term)

set(SOURCE_FILES lexer.cpp Tokenstream.cpp complexity_analyzer/ExecutionPath.cpp CodeParser.cpp global_information.cpp complexity_analyzer/virtual_math_term.cpp complexity_analyzer/ComplexityHelper.cpp
        complexity_analyzer/recursive_analysis.cpp complexity_analyzer/LogicTerm.cpp LocalVariableManager.cpp abstract_syntax_tree/ASTWhileLoopNode.cpp abstract_syntax_tree/ASTTree.hpp
        abstract_syntax_tree/ASTStatementNode.hpp abstract_syntax_tree/ASTRootNode.cpp abstract_syntax_tree/ASTReturnNode.cpp
        abstract_syntax_tree/ASTNode.cpp abstract_syntax_tree/ASTLogicalNode.cpp abstract_syntax_tree/ASTIfElseNode.cpp
        abstract_syntax_tree/ASTFunctionNode.cpp abstract_syntax_tree/ASTComparisonNode.cpp abstract_syntax_tree/ASTCallNode.cpp
        abstract_syntax_tree/ASTCalculationNode.cpp abstract_syntax_tree/ASTAssignmentNode.cpp abstract_syntax_tree/ASTCommentNode.cpp abstract_syntax_tree/ASTClassNode.cpp
        GlobalVariableManager.cpp abstract_syntax_tree/ASTVariableNode.cpp)



add_executable(main main.cpp ${SOURCE_FILES} abstract_syntax_tree/ASTVariableNode.cpp)

add_executable(compiler compiler.cpp ${SOURCE_FILES} abstract_syntax_tree/ASTVariableNode.cpp)

add_executable(analyzer complexity_analyzer/analyzer.cpp ${SOURCE_FILES} abstract_syntax_tree/ASTVariableNode.cpp)