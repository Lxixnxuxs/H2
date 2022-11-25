include_directories(expression_test ../)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(parser_test parser_test.cpp)

add_executable(expression_test expression_test.cpp)

target_compile_definitions(parser_test PRIVATE TEST_FILE_PATH=${CMAKE_SOURCE_DIR}/test/resources/test_program.txt)

target_link_libraries(
        parser_test
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(parser_test)