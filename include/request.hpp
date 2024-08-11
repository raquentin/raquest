#pragma once

#include <string>
#include <vector>
#include <curl/curl.h>

class Request {
public:
    Request(const std::string& method, const std::string& url);
    void add_header(const std::string& header);
    void set_json_body(const std::string& json);
    void execute() const;

private:
    std::string method;
    std::string url;
    std::vector<std::string> headers;
    std::string json_body;
    mutable std::string response_data;
    CURL* curl;

    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
};
