#include "request.hpp"
#include <iostream>

Request::Request(const std::string& method, const std::string& url) : method(method), url(url), curl(curl_easy_init())
{}

void Request::add_header(const std::string& header) {
    headers.push_back(header);
}

void Request::execute() const {
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        struct curl_slist *curl_headers = nullptr;
        for (const auto& header : headers) {
            curl_headers = curl_slist_append(curl_headers, header.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);


        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_slist_free_all(curl_headers);
        curl_easy_cleanup(curl);
    }
}
