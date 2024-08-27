#pragma once

#include "error.hpp"
#include <expected>
#include <optional>
#include <string>
#include <vector>

enum class TokenType {
  OpenBracket,
  CloseBracket,
  Equals,
  Identifier,
  Colon,
  StringLiteral,
  Newline,
  EndOfFile,
};

struct Token {
  TokenType type;
  std::string value;
  int line_number;
  int column_number;
};

class Lexer {
public:
  Lexer(const std::string &input);
  std::expected<std::vector<Token>, std::vector<Error>> tokenize();
  std::vector<Error> errors;

private:
  std::string input;
  size_t position;
  int line_number;
  int column_number;

  void skip_whitespace_and_comments();
  std::optional<Token> tokenize_identifier();
  std::optional<Token> tokenize_string_literal();
  std::optional<Token> next_token();
};
