cmake_minimum_required(VERSION 4.0)
project(AUTOMATA2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(AUTOMATA2
        main.cpp
        Dialogue.cpp
        RegexParser.cpp
        SyntaxTree.cpp
        NFA.cpp
        Thompson.cpp
)

target_include_directories(AUTOMATA2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
