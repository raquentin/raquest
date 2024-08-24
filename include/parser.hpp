#pragma once
#include <expected>
#include "lexer.hpp"
#include "request.hpp"

class Parser {
public:
  static Request parse_file(const std::string &file_path);

private:
  std::expected<void, ParseError> parse_method(Lexer &lexer, Request &request);
  std::expected<void, ParseError> parse_headers(Lexer &lexer, Request &request);
  std::expected<void, ParseError> parse_body(Lexer &lexer, Request &request);
};
