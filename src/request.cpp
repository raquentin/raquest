#include "request.hpp"
#include <iostream>

Request::Request(const std::string &method, const std::string &url)
    : method(method), url(url), curl(curl_easy_init()) {}

void Request::add_header(const std::string &header) {
  headers.push_back(header);
}

void Request::set_json_body(const std::string &json) { json_body = json; }

size_t Request::WriteCallback(void *contents, size_t size, size_t nmemb,
                              void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

void Request::execute() const {
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    struct curl_slist *curl_headers = nullptr;
    for (const auto &header : headers) {
      curl_headers = curl_slist_append(curl_headers, header.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);

    if (method == "POST" || method == "PUT" || method == "PATCH") {
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
                << std::endl;
    } else {
      std::cout << response_data << std::endl;
    }

    curl_slist_free_all(curl_headers);
    curl_easy_cleanup(curl);
  }
}

const std::string &Request::get_method() const { return method; }

const std::string &Request::get_url() const { return url; }

const std::vector<std::string> &Request::get_headers() const { return headers; }

const std::string &Request::get_json_body() const { return json_body; }

const std::string &Request::get_response_data() const { return response_data; }
