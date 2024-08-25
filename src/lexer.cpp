#include "lexer.hpp"
#include <cctype>
#include <optional>
#include <vector>

Lexer::Lexer(const std::string &input)
    : input(input), position(0), line_number(1) {}

void Lexer::skip_whitespace_and_comments() {
  while (position < input.size() && std::isspace(input[position])) {
    if (input[position] == '\n') {
      line_number++;
      column_number = 1;
    } else {
      column_number++;
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

std::optional<Token> Lexer::tokenize_identifier() {
  size_t start = position;
  int start_column = column_number;

  while (position < input.size() && (std::isalnum(input[position])) ||
         input[position] == '_') {
    position++;
    column_number++;
  }

  if (start == position) {
    return std::nullopt;
  }

  return Token{TokenType::Identifier, input.substr(start, position - start),
               line_number, start_column};
}

std::optional<Token> Lexer::tokenize_string_literal() {
  if (input[position] != '"')
    return std::nullopt;

  int start_column = column_number;
  position++;
  column_number++;

  size_t start = position;
  while (position < input.size() && input[position] != '"') {
    if (input[position] == '\n') {
      errors.push_back(Error{ErrorType::UnterminatedStringLiteral,
                             "Unterminated string literal", line_number,
                             start_column});
    }
    position++;
    column_number++;
  }

  if (position >= input.size() && input[position] != '"') {
    errors.push_back(Error{ErrorType::UnterminatedStringLiteral,
                           "Unterminated string literal", line_number,
                           start_column});
  }

  std::string value = input.substr(start, position - start);
  position++;
  column_number++;

  return Token{TokenType::StringLiteral, value, line_number, start_column};
}

std::optional<Token> Lexer::next_token() {
  skip_whitespace_and_comments();

  if (position >= input.size()) {
    return Token{TokenType::EndOfFile, "", line_number, column_number};
  }

  char curr_char = input[position];
  int start_column = column_number;

  if (curr_char == '[') {
    position++;
    column_number++;
    return Token{TokenType::OpenBracket, "[", line_number, start_column};
  } else if (curr_char == ']') {
    position++;
    column_number++;
    return Token{TokenType::CloseBracket, "]", line_number, start_column};
  } else if (curr_char == '=') {
    position++;
    column_number++;
    return Token{TokenType::Equals, "=", line_number, start_column};
  } else if (curr_char == ':') {
    position++;
    column_number++;
    return Token{TokenType::Colon, ":", line_number, start_column};
  } else if (curr_char == '\n') {
    position++;
    line_number++;
    column_number = 1;
    return Token{TokenType::Newline, "\n", line_number, start_column};
  } else if (curr_char == '"') {
    return tokenize_string_literal();
  } else if (std::isalpha(curr_char) || curr_char == '_') {
    return tokenize_identifier();
  } else {
    errors.push_back(Error{ErrorType::UnexpectedCharacter,
                           "Unexpected character: ", line_number,
                           start_column});
    position++;
    column_number++;
    return std::nullopt;
  }
}

std::expected<std::vector<Token>, std::vector<Error>> Lexer::tokenize() {
  std::vector<Token> tokens;

  while (position < input.size()) {
    auto token = next_token();
    if (token.has_value()) {
      tokens.push_back(token.value());
    } else if (!errors.empty()) {
      return std::unexpected(errors);
    }
  }

  tokens.push_back(Token{TokenType::EndOfFile, "", line_number, column_number});
  return tokens;
}
