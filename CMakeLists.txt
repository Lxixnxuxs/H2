cmake_minimum_required(VERSION 3.22)
project(H2)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(test)

add_executable(H2 main.cpp Expression.hpp)
