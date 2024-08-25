#pragma once
#include "lexer.hpp"
#include "request.hpp"
#include <expected>

enum class HttpMethod { GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS, UNKNOWN };

class Parser {
public:
  std::expected<Request, std::vector<Error>>
  parse(const std::vector<Token> &tokens);

private:
  std::vector<Error> errors;

  std::optional<Token> expect_token(const std::vector<Token> &tokens,
                                    size_t &curr, TokenType expected_type);

  void parse_method_and_url(const std::vector<Token> &tokens, size_t &curr,
                            Request &request);
  void parse_section(const std::vector<Token> &tokens, size_t &curr,
                     Request &request);
  void parse_headers(const std::vector<Token> &tokens, size_t &curr,
                     Request &request);
  void parse_body(const std::vector<Token> &tokens, size_t &curr,
                  Request &request);
};
