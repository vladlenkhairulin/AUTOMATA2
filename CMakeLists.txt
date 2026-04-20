
#include <fstream>
cmake_minimum_required(VERSION 3.0)
project(AUTOMATA2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(AUTOMATA2
        main.cpp
        DFAversion.cpp
        RegexParser.cpp
        SyntaxTree.cpp
        Thompson.cpp
        StateElimination.cpp
)

target_include_directories(AUTOMATA2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
