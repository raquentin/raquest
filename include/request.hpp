#pragma once

#include <curl/curl.h>
#include <string>
#include <vector>

class Request {
public:
  Request(const std::string &method, const std::string &url);
  void add_header(const std::string &header);
  void set_json_body(const std::string &json);
  void execute() const;

  const std::string &get_method() const;
  const std::string &get_url() const;
  const std::vector<std::string> &get_headers() const;
  const std::string &get_json_body() const;
  const std::string &get_response_data() const;

private:
  std::string method;
  std::string url;
  std::vector<std::string> headers;
  std::string json_body;
  mutable std::string response_data;
  CURL *curl;

  static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                              void *userp);
};
