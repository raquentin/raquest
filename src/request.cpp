#include "request.hpp"
#include "assert.hpp"
#include "errors/error_manager.hpp"
#include "errors/runtime_error.hpp"
#include "printer.hpp"
#include <cstddef>
#include <curl/easy.h>

Request::Request(const std::string &file_name, const std::string &method,
                 const std::string &url, ErrorManager &error_manager)
    : file_name_(file_name), method_(method), url_(url),
      error_manager_(error_manager), curl_(curl_easy_init()),
      assertion_set_(AssertionSet{}) {
  if (!curl_) {
    error_manager_.add_error(std::make_shared<RuntimeError>(
        file_name_, "failed to initiate libcurl", ErrorSeverity::Error,
        RuntimeErrorType::Curl));
  }
}

void Request::set_method(const std::string &method) { this->method_ = method; }

void Request::set_url(const std::string &url) { this->url_ = url; }

void Request::add_header(const std::string &key, const std::string &value) {
  headers_.push_back(key + ": " + value);
}

void Request::set_body(const std::string &json) { body_ = json; }

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

std::optional<Response> Request::execute() const {
  print_running(file_name_);

  struct curl_slist *curl_headers = nullptr;
  for (const auto &header : headers_) {
    curl_headers = curl_slist_append(curl_headers, header.c_str());
  }

  curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, curl_headers);
  curl_easy_setopt(curl_, CURLOPT_URL, url_.c_str());
  curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, method_.c_str());

  if (!body_.empty()) {
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, body_.c_str());
  }

  Response response;

  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response_data_);

  curl_easy_setopt(curl_, CURLOPT_HEADERFUNCTION, HeaderCallback);
  curl_easy_setopt(curl_, CURLOPT_HEADERDATA, &response);

  CURLcode res = curl_easy_perform(curl_);
  if (res != CURLE_OK) {
    error_manager_.add_error(std::make_shared<RuntimeError>(
        file_name_, std::string(curl_easy_strerror(res)), ErrorSeverity::Error,
        RuntimeErrorType::Curl));
  }

  long http_code = 0;
  curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &http_code);
  response.parse_status_code(http_code);
  response.set_body(response_data_);

  curl_slist_free_all(curl_headers);
  curl_easy_cleanup(curl_);

  return response;
}

const std::string &Request::get_method() const { return method_; }

const std::string &Request::get_url() const { return url_; }

const std::vector<std::string> &Request::get_headers() const { return headers_; }

const std::string &Request::get_body() const { return body_; }

const std::string &Request::get_response_data() const { return response_data_; }
