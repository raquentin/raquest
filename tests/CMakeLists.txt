set(TEST_SOURCE_FILES tautology_test.cpp parser_test.cpp)

add_executable(raquest_test ${TEST_SOURCE_FILES})

target_link_libraries(raquest_test gtest gtest_main raquestlib)

add_test(NAME test COMMAND raquest_test)

target_link_libraries(raquest_test CURL::libcurl CLI11::CLI11
                      nlohmann_json::nlohmann_json)

include(GoogleTest)
gtest_discover_tests(raquest_test)
