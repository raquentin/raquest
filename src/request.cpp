#include "request.hpp"
#include <cstddef>
#include <curl/easy.h>
#include <iostream>

Request::Request(const std::string &method, const std::string &url)
    : method(method), url(url), curl(curl_easy_init()) {
  if (!curl) {
    throw std::runtime_error("failed to initialize curl");
  }
}

void Request::set_method(const std::string &method) { this->method = method; }

void Request::set_url(const std::string &url) { this->url = url; }

void Request::add_header(const std::string &key, const std::string &value) {
  // TODO: smart validation of key and value mapping against stds
  headers.push_back(key + ": " + value);
}

void Request::set_body(const std::string &json) { body = json; }

void Request::add_status_code_assertion(const std::vector<int> &codes) {
  auto assertion = Assertion::create().status_codes(codes);
  assertions.push_back(std::make_shared<Assertion>(assertion));
}

void Request::add_header_assertion(const std::string &key,
                                   const std::string &value) {
  auto assertion = Assertion::create().header(key, value);
  assertions.push_back(std::make_shared<Assertion>(assertion));
}

void Request::add_json_field_assertion(const std::string &field,
                                       const std::string &pattern) {
  auto assertion = Assertion::create().json_field(field, pattern);
  assertions.push_back(std::make_shared<Assertion>(assertion));
}

void Request::check_assertions(const Response &response) const {
  for (const auto &assertion : assertions) {
  if (auto err = assertion->validate(response)) {
      std::cerr << "Assertion failed: " << err->message << std::endl;
    }
  }
}

size_t Request::WriteCallback(void *contents, size_t size, size_t nmemb,
                              void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

size_t Request::HeaderCallback(char *buffer, size_t size, size_t nitems,
                               void *userdata) {
  Response *response = static_cast<Response *>(userdata);
  size_t header_size = size * nitems;
  response->parse_header(std::string(buffer, header_size));
  return header_size;
}

void Request::execute() const {
  if (!curl) {
    throw std::runtime_error("curl is not initialized");
  }

  struct curl_slist *curl_headers = nullptr;
  for (const auto &header : headers) {
    curl_headers = curl_slist_append(curl_headers, header.c_str());
  }

  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());

  if (!body.empty()) {
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
  }

  Response response;

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

  curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
  curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    throw std::runtime_error("CURL request failed: " +
                             std::string(curl_easy_strerror(res)));
  }

  long http_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
  response.parse_status_code(http_code);
  response.set_body(response_data);

  curl_slist_free_all(curl_headers);
  curl_easy_cleanup(curl);

  std::cout << response_data << std::endl;

  check_assertions(response);
}

const std::string &Request::get_method() const { return method; }

const std::string &Request::get_url() const { return url; }

const std::vector<std::string> &Request::get_headers() const { return headers; }

const std::string &Request::get_body() const { return body; }

const std::string &Request::get_response_data() const { return response_data; }
