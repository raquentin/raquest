#pragma once
#include <map>
#include <optional>
#include <string>

class Response {
public:
  int get_status_code() const;
  std::optional<std::string> get_header_value(const std::string &key) const;
  std::optional<std::string> get_json_field(const std::string &key) const;
  std::string get_body() const;

  void parse_status_code(long code);
  void parse_header(const std::string &header_line);
  void set_body(const std::string &data);

private:
  int status_code;
  std::map<std::string, std::string> headers;
  std::string body;
};
