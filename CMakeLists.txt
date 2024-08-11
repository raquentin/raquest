cmake_minimum_required(VERSION 3.22)
project(raquest)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMAND ON)

add_definitions(-DSOME_DEFINITION)
add_subdirectory(src)
