cmake_minimum_required(VERSION 3.22)
project(H2)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(test)
add_subdirectory(abstract_syntax_tree)
include_directories(term)

set(SOURCE_FILES lexer.cpp Tokenstream.cpp ExecutionPath.cpp CodeParser.cpp global_information.cpp virtual_math_term.cpp ComplexityHelper.cpp
        recursive_analysis.cpp LogicTerm.cpp LocalVariableManager.cpp abstract_syntax_tree/ASTWhileLoopNode.cpp abstract_syntax_tree/ASTTree.hpp
        abstract_syntax_tree/ASTStatementNode.hpp abstract_syntax_tree/ASTRootNode.cpp abstract_syntax_tree/ASTReturnNode.cpp
        abstract_syntax_tree/ASTNode.cpp abstract_syntax_tree/ASTLogicalNode.cpp abstract_syntax_tree/ASTIfElseNode.cpp
        abstract_syntax_tree/ASTFunctionNode.cpp abstract_syntax_tree/ASTComparisonNode.cpp abstract_syntax_tree/ASTCallNode.cpp
        abstract_syntax_tree/ASTCalculationNode.cpp abstract_syntax_tree/ASTAssignmentNode.cpp)

add_executable(main main.cpp ${SOURCE_FILES})

add_executable(compiler compiler.cpp ${SOURCE_FILES})

add_executable(analyzer analyzer.cpp ${SOURCE_FILES})