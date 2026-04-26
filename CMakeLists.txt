
cmake_minimum_required(VERSION 3.0)
project(AUTOMATA2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(regexlib
        src/DFAversion.cpp
        src/RegexParser.cpp
        src/SyntaxTree.cpp
        src/Thompson.cpp
        src/StateElimination.cpp
        src/Regex.cpp
)

target_include_directories(regexlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(regexlib PRIVATE --coverage -O0 -g)
target_link_options(regexlib PRIVATE --coverage)

add_executable(run main.cpp)
target_link_libraries(run PRIVATE regexlib)
enable_testing()
add_subdirectory(tests)
