#pragma once

#include "assert.hpp"
#include "errors/error_manager.hpp"
#include "response.hpp"
#include <curl/curl.h>
#include <string>
#include <vector>

class Request {
public:
  Request(const std::string &file_name, const std::string &method,
          const std::string &url, ErrorManager &error_manager);

  void set_method(const std::string &method);
  void set_url(const std::string &url);
  void add_header(const std::string &key, const std::string &value);
  void set_body(const std::string &json);
  void set_assertion_set(AssertionSet assertion_set) const { assertion_set_ = assertion_set; }

  std::optional<Response> execute() const;

  const std::string &get_method() const;
  const std::string &get_url() const;
  const std::vector<std::string> &get_headers() const;
  const std::string &get_body() const;
  const std::string &get_response_data() const;
  const AssertionSet &get_assertion_set() const { return assertion_set_; }

private:
  std::string file_name_;
  std::string method_;
  std::string url_;
  std::vector<std::string> headers_;
  std::string body_;
  mutable AssertionSet assertion_set_;
  mutable std::string response_data_;
  CURL *curl_;

  ErrorManager &error_manager_;

  static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                              void *userp);
  static size_t HeaderCallback(char *buffer, size_t size, size_t nitems,
                               void *userdata);
};
