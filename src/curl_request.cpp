#include "curl_request.hpp"
#include "curl/curl.h"
#include "errors/curl_error.hpp"
#include "printer.hpp"
#include <cstddef>
#include <curl/easy.h>

CurlRequest::CurlRequest(const std::string &file_name) : file_name_(file_name) {
    curl_handle_ = curl_easy_init();
    curl_easy_setopt(curl_handle_, CURLOPT_HEADERFUNCTION, HeaderCallback);
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEFUNCTION, WriteCallback);
};

std::expected<CurlResponse, CurlError> CurlRequest::execute() const {
    struct curl_slist *curl_headers = nullptr;
    for (const auto &header : headers_) {
        curl_headers = curl_slist_append(curl_headers, header.c_str());
    }

    curl_easy_setopt(curl_handle_, CURLOPT_HTTPHEADER, curl_headers);
    curl_easy_setopt(curl_handle_, CURLOPT_URL, url_.c_str());
    curl_easy_setopt(curl_handle_, CURLOPT_CUSTOMREQUEST, method_.c_str());

    if (!body_.empty()) {
        curl_easy_setopt(curl_handle_, CURLOPT_POSTFIELDS, body_.c_str());
    }

    CurlResponse response;

    curl_easy_setopt(curl_handle_, CURLOPT_WRITEDATA, &response_data_);

    curl_easy_setopt(curl_handle_, CURLOPT_HEADERDATA, &response);

    CURLcode res = curl_easy_perform(curl_handle_);
    if (res != CURLE_OK) {
        return std::unexpected(
            CurlError(file_name_, CurlError::Type::UnitializedCurl));
    }

    long http_code = 0;
    curl_easy_getinfo(curl_handle_, CURLINFO_RESPONSE_CODE, &http_code);
    response.parse_status_code(http_code);
    response.set_body(response_data_);

    curl_slist_free_all(curl_headers);
    curl_easy_cleanup(curl_handle_);

    return response;
}

size_t CurlRequest::WriteCallback(void *contents, size_t size, size_t nmemb,
                                  void *userp) {
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

size_t CurlRequest::HeaderCallback(char *buffer, size_t size, size_t nitems,
                                   void *userdata) {
    CurlResponse *response = static_cast<CurlResponse *>(userdata);
    size_t header_size = size * nitems;
    response->parse_header(std::string(buffer, header_size));
    return header_size;
}
