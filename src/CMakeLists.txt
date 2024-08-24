set(SOURCE_FILES parser.cpp request.cpp lexer.cpp)

add_library(raquestlib ${SOURCE_FILES})

add_executable(raquest main.cpp)
target_link_libraries(raquest raquestlib)

# curl
find_package(CURL REQUIRED)
target_link_libraries(raquest CURL::libcurl)
