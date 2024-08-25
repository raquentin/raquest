#include "parser.hpp"
#include "CLI11.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

const std::unordered_map<std::string, HttpMethod> http_methods = {
    {"GET", HttpMethod::GET},        {"POST", HttpMethod::POST},
    {"PUT", HttpMethod::PUT},        {"DELETE", HttpMethod::DELETE},
    {"PATCH", HttpMethod::PATCH},    {"HEAD", HttpMethod::HEAD},
    {"OPTIONS", HttpMethod::OPTIONS}};

std::expected<Request, std::vector<ParseError>>
Parser::parse(const std::vector<Token> &tokens) {
  Request request("", "");
  size_t curr = 0;

  // parse method and url
  auto method_token = expect_token(tokens, curr, TokenType::Method);
  if (!method_token.has_value()) {
    errors.push_back(ParseError{ErrorType::SemanticError,
                                "Expected HTTP method",
                                tokens[curr].line_number});
  }

  auto url_token = expect_token(tokens, curr, TokenType::Url);
  if (!url_token.has_value()) {
    errors.push_back(ParseError{ErrorType::SemanticError,
                                "Expected URL after method",
                                tokens[curr].line_number});
  }

  request = Request(method_token->value, url_token->value);

  // parse subsequent
  while (curr < tokens.size()) {
    parse_section(tokens, curr, request);
  }

  if (errors.empty()) {
    return std::unexpected(errors);
  }

  return request;
}

std::optional<Token> Parser::expect_token(const std::vector<Token> &tokens,
                                          size_t &current,
                                          TokenType expected_type) {
  if (current >= tokens.size() || tokens[current].type != expected_type) {
    return std::nullopt;
  }
  return tokens[current++];
}

void Parser::parse_section(const std::vector<Token> &tokens, size_t &curr,
                           Request &request) {
  auto section_token = expect_token(tokens, curr, TokenType::HeaderSection);
  if (!section_token.has_value()) {
  }
