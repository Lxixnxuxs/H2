cmake_minimum_required(VERSION 3.22)
project(H2)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(test)
add_subdirectory(abstract_syntax_tree)
include_directories(term)

add_executable(main main.cpp lexer.cpp Tokenstream.cpp)
add_executable(compiler compiler.cpp lexer.cpp Tokenstream.cpp)
add_executable(analyzer analyzer.cpp lexer.cpp Tokenstream.cpp complexity_term.cpp)