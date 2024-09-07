#pragma once

#include <curl/curl.h>
#include <string>
#include <vector>

class Request {
public:
  Request(const std::string &method, const std::string &url);

  void set_method(const std::string &method);
  void set_url(const std::string &url);
  void add_header(const std::string &key, const std::string &value);
  void set_body(const std::string &json);
  // TODO: assertions won't be strings, make assertions.cpp
  void add_assertion(const std::string &assertion);

  const std::string &get_method() const;
  const std::string &get_url() const;
  const std::vector<std::string> &get_headers() const;
  const std::string &get_body() const;
  const std::string &get_response_data() const;
  const std::vector<std::string> &get_assertions() const;

  void execute() const;

private:
  std::string method;
  std::string url;
  std::vector<std::string> headers;
  std::string body;
  mutable std::string response_data;
  std::vector<std::string> assertions;
  CURL *curl;

  static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                              void *userp);
};
