#pragma once

#include <string>
#include <vector>
#include <curl/curl.h>

class Request {
public:
    Request(const std::string& method, const std::string& url);
    void add_header(const std::string& header);
    void execute() const;

private:
    std::string method;
    std::string url;
    std::vector<std::string> headers;
    CURL* curl;
};
