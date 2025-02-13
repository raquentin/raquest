set(SOURCE_FILES
    errors/assertion_error.cpp
    errors/curl_error.cpp
    errors/file_error.cpp
    errors/parser_error.cpp
    assertion_set.cpp
    config.cpp
    curl_request.cpp
    curl_response.cpp
    parser.cpp
    printer.cpp
    raquest.cpp
)

add_library(raquestlib ${SOURCE_FILES})

target_link_libraries(raquestlib ThreadPool CURL::libcurl nlohmann_json::nlohmann_json
                      fmt::fmt)

add_executable(raquest main.cpp)


target_link_libraries(raquest raquestlib)

target_link_libraries(raquest CLI11::CLI11)
