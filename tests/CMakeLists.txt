set(TEST_SOURCE_FILES tautology_test.cpp parser_test.cpp)

add_executable(raquest_test ${TEST_SOURCE_FILES})

target_link_libraries(raquest_test gtest gtest_main raquestlib)

add_test(NAME test COMMAND raquest_test)

# curl
find_package(CURL REQUIRED)
target_link_libraries(raquest_test CURL::libcurl)
