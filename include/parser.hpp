#pragma once
#include "errors/error_manager.hpp"
#include "request.hpp"
#include <expected>
#include <optional>
#include <string>

enum class HttpMethod { GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS, UNKNOWN };

class Parser {
public:
  Parser(const std::string &file_name, ErrorManager &error_manager);
  std::optional<std::shared_ptr<Request>> parse();

private:
  std::string input;
  std::string file_name;
  size_t position;
  int line_number;
  int column_number;

  ErrorManager &error_manager;

  std::optional<std::string> next_line();
  void parse_request(Request &request);
  void parse_headers(Request &request);
  void parse_body(Request &request);
  AssertionSet parse_assertions();
};
