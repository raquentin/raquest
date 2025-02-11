set(SOURCE_FILES
    parser.cpp
    curl_request.cpp
    curl_response.cpp
    assertion_set.cpp
    raquest.cpp
    errors/parser_error.cpp
    errors/curl_error.cpp
    errors/file_error.cpp
    errors/assertion_error.cpp)

add_library(raquestlib ${SOURCE_FILES})

target_link_libraries(raquestlib CURL::libcurl nlohmann_json::nlohmann_json
                      fmt::fmt)

add_executable(raquest main.cpp)
target_link_libraries(raquest raquestlib)

target_link_libraries(raquest CLI11::CLI11)
