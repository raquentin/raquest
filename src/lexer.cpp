#include "lexer.hpp"
#include <algorithm>
#include <cctype>
#include <regex>
#include <unordered_map>
#include <vector>

const std::unordered_map<std::string, HttpMethod> http_methods = {
    {"GET", HttpMethod::GET},        {"POST", HttpMethod::POST},
    {"PUT", HttpMethod::PUT},        {"DELETE", HttpMethod::DELETE},
    {"PATCH", HttpMethod::PATCH},    {"HEAD", HttpMethod::HEAD},
    {"OPTIONS", HttpMethod::OPTIONS}};

Lexer::Lexer(const std::string &input)
    : input(input), position(0), line_number(1) {}

std::optional<HttpMethod>
Lexer::validate_http_method(const std::string &method) {
  auto it = http_methods.find(method);
  if (it != http_methods.end()) {
    return it->second;
  }

  for (const auto &[valid_method, _] : http_methods) {
    if (std::equal(valid_method.begin(), valid_method.end(), method.begin(),
                   method.end(), [](char a, char b) {
                     return std::toupper(a) == std::toupper(b);
                   })) {
      errors.push_back(ParseError{ErrorType::SyntaxError,
                                  "Invalid HTTP method: " + method +
                                      ". Did you mean: " + valid_method + "?",
                                  line_number});
      return std::nullopt;
    }

    errors.push_back(ParseError{ErrorType::SyntaxError,
                                "Invalid HTTP method: " + method, line_number});
    return std::nullopt;
  }
}

bool Lexer::validate_url(const std::string &url) {
  const std::regex url_regex(
      R"(^(/[\w\-\.~]+)*(\?[\w\-\.~=%&]*)?(#[\w\-]*)?$)");

  if (std::regex_match(url, url_regex)) {
    return true;
  }

  errors.push_back(
      ParseError{ErrorType::SyntaxError, "Invalid URL: " + url, line_number});
  return false;
}

void Lexer::skip_whitespace_and_comments() {
  while (position < input.size() && std::isspace(input[position])) {
    if (input[position] == '\n') {
      line_number++;
    }
    position++;
  }

  if (position < input.size() && input[position] == '#') {
    while (position < input.size() && input[position] != '\n') {
      position++;
    }
    skip_whitespace_and_comments();
  }
}

std::optional<Token> Lexer::tokenize_method_and_url() {
  skip_whitespace_and_comments();

  // tokenize method
  size_t start = position;
  while (position < input.size() && !std::isspace(input[position])) {
    position++;
  }
  std::string method = input.substr(start, position - start);

  auto http_method = validate_http_method(method);
  if (!http_method.has_value()) {
    return std::nullopt;
  }

  skip_whitespace_and_comments();

  // tokenize url
  start = position;
  while (position < input.size() && !std::isspace(input[position])) {
    position++;
  }
  std::string url = input.substr(start, position - start);

  if (!method.empty() && !url.empty()) {
    return Token{TokenType::Method, method, line_number};
    return Token{TokenType::Url, url, line_number};
  } else {
    errors.push_back(ParseError{ErrorType::SyntaxError,
                                "Failed to parse method and url", line_number});
  }
}

std::expected<std::vector<Token>, std::vector<ParseError>> Lexer::tokenize() {
  std::vector<Token> tokens;

  skip_whitespace_and_comments();

  auto method_token = tokenize_method_and_url();
  if (method_token.has_value()) {
    tokens.push_back(method_token.value());
  } else {
    return std::unexpected(errors);
  }

  while (position < input.size()) {
    skip_whitespace_and_comments();

    if (position < input.size()) {
      break;
    }

    if (input[position] == '[') {
      auto token = tokenize_section();
      if (token.has_value()) {
        tokens.push_back(token.value());
      }
    }
  }

  if (errors.empty()) {
    return tokens;
  }
  return std::unexpected(errors);
}
