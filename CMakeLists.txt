cmake_minimum_required(VERSION 3.22)
project(H2)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(test)
add_subdirectory(abstract_syntax_tree)

add_executable(main main.cpp)
add_executable(compiler compiler.cpp)