#pragma once

#include "curl_response.hpp"
#include "errors/curl_error.hpp"
#include <expected>
#include <string>
#include <vector>

struct CurlRequest {
    std::string file_name_;
    std::string method_;
    std::string url_;
    std::vector<std::string> headers_;
    std::string body_;
    mutable std::string response_data_;

    CurlRequest(const std::string &file_name_);

    std::expected<CurlResponse, CurlError> execute() const;

    static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                                void *userp);
    static size_t HeaderCallback(char *buffer, size_t size, size_t nitems,
                                 void *userdata);
};
