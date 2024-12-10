#pragma once
#include "errors/error.hpp"
#include "errors/parser_error.hpp"
#include "request.hpp"
#include <expected>
#include <optional>
#include <string>
#include <vector>

enum class HttpMethod { GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS, UNKNOWN };

class Parser {
public:
  Parser(const std::string &file_name);
  std::expected<Request, std::vector<ParserError>> parse();
  std::vector<ParserError> errors;

private:
  std::string input;
  std::string file_name;
  size_t position;
  int line_number;
  int column_number;

  std::optional<std::string> next_line();
  void parse_request(Request &request);
  void parse_headers(Request &request);
  void parse_body(Request &request);
  void parse_assertion(Request &request);
};
