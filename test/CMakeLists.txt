include_directories(expression_test ../)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(lexer_test lexer_test.cpp)
add_executable(tokenstream_test tokenstream_test.cpp)
add_executable(code_parser_tests code_parser_tests.cpp)
add_executable(file_editor_test file_editor_test.cpp ../FileEditor.cpp)


target_link_libraries(lexer_test GTest::gtest_main)
target_link_libraries(tokenstream_test GTest::gtest_main)
target_link_libraries(code_parser_tests GTest::gtest_main)
target_link_libraries(file_editor_test GTest::gtest_main)


#target_compile_definitions(parser_test PRIVATE TEST_FILE_PATH=${CMAKE_SOURCE_DIR}/test/resources/test_program.txt)

include(GoogleTest)

gtest_discover_tests(lexer_test)
gtest_discover_tests(tokenstream_test)
gtest_discover_tests(code_parser_tests)
gtest_discover_tests(file_editor_test)
