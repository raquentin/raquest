#pragma once
#include "error.hpp"
#include "request.hpp"
#include <expected>
#include <optional>
#include <string>
#include <vector>

enum class HttpMethod { GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS, UNKNOWN };

class Parser {
public:
  Parser(const std::string &input);
  std::expected<Request, std::vector<Error>> parse();

private:
  std::string input;
  size_t position;
  int line_number;
  int column_number;
  std::vector<Error> errors;

  std::optional<std::string> next_line();
  void parse_request(Request &request);
  void parse_headers(Request &request);
  void parse_body(Request &request);
  void parse_assertions(Request &request);
};
