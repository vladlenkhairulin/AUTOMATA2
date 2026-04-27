add_library(catch2 STATIC
        catch2/catch_amalgamated.cpp
)

target_include_directories(catch2 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/catch2
)

add_executable(unit_tests tests.cpp)

target_link_libraries(unit_tests PRIVATE
        regexlib
        catch2
)

enable_testing()
add_test(NAME AllTests COMMAND unit_tests)