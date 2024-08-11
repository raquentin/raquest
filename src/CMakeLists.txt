add_executable(raquest main.cpp parser.cpp request.cpp)
target_include_directories(raquest PUBLIC ${PROJECT_SOURCE_DIR}/include)

# curl
find_package(CURL REQUIRED)
target_link_libraries(raquest CURL::libcurl)
