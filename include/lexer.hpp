#pragma once

#include "error.hpp"
#include <expected>
#include <string>
#include <vector>

enum class HttpMethod { GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS, UNKNOWN };

enum class TokenType {
  OpenBracket,
  CloseBracket,
  Colon,
  Equals,
  Identifier,
  StringLiteral,
  Newline,
  EndOfFile,
  
};

struct Token {
  TokenType type;
  std::string value;
  int line_number;
};

class Lexer {
public:
  Lexer(const std::string &input);
  std::expected<std::vector<Token>, std::vector<ParseError>> tokenize();

private:
  std::string input;
  size_t position;
  int line_number;

  std::vector<ParseError> errors;

  void initialize_http_methods();

  void skip_whitespace_and_comments();

  /// Tokenized the first line of a raq file, containing the method (GET, POST,
  /// ..) and the url
  std::optional<Token> tokenize_method_and_url();

  /// Intelligently validates and reports errors for http methods
  std::optional<HttpMethod> validate_http_method(const std::string &method);

  bool validate_url(const std::string &url);

  /// Tokenizes section titles and their subtypes, e.g. [Headers] or [Body
  /// type=json]
  std::optional<Token> tokenize_section();

  /// Tokenizes the headers under a [Headers] section
  std::optional<Token> tokenize_headers();

  /// Tokenizes the body under a [Body] section
  /// Note: we don't tokenizing json/xml/etc., just ident body type and prep for
  /// libcurl
  std::optional<Token> tokenize_body();
};
