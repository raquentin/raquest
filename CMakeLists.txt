cmake_minimum_required(VERSION 3.10)
project(raquest)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(include)

# Source files
file(GLOB SOURCES "src/*.cpp")

# Executable
add_executable(raquest ${SOURCES})

# Libraries (e.g., cURL)
find_package(CURL REQUIRED)
target_link_libraries(raquest CURL::libcurl)


enable_testing()

# Add GoogleTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
add_executable(runTests tests/main.cpp)
target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)

add_test(NAME runTests COMMAND runTests)
