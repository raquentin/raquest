#include "lexer.hpp"
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <vector>

Lexer::Lexer(const std::string &input)
    : input(input), position(0), line_number(1) {}

std::expected<std::vector<Token>, std::vector<ParseError>> Lexer::tokenize() {
  std::vector<Token> tokens;

  skip_whitespace_and_comments();
  auto method_token = tokenize_method();
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

std::optional<Token> Lexer::tokenize_method() {
  size_t start = position;

  // tokenize method
  while (position < input.size() && !std::isspace(input[position])) {
    position++;
  }
  std::string method = input.substr(start, position - start);

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
    errors.push_back(ParseError{"failed to parse method and url", line_number})
  }
}
