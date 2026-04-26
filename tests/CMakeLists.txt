find_package(Catch2 3 REQUIRED)

add_executable(unit_tests tests.cpp)

target_link_libraries(unit_tests PRIVATE
        regexlib
        Catch2::Catch2WithMain
)

include(CTest)
add_test(NAME AllTests COMMAND unit_tests)