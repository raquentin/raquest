enable_testing()

set(TEST_SOURCE_FILES main.cpp)

add_executable(raquest_test ${TEST_SOURCE_FILES})

target_link_libraries(raquest_test gtest gtest_main raquestlib)

add_test(NAME test COMMAND raquest_test)
