#include "parser.hpp"
#include <sstream>

std::expected<Request, std::vector<Error>>
Parser::parse(const std::vector<Token> &tokens) {
  Request request("", "");
  size_t curr = 0;

  parse_reqline(tokens, curr, request);

  while (curr < tokens.size()) {
    parse_section(tokens, curr, request);
  }

  if (!errors.empty()) {
    return std::unexpected(errors);
  }

  return request;
}

std::optional<Token> Parser::expect_token(const std::vector<Token> &tokens,
                                          size_t &current,
                                          TokenType expected_type) {
  if (current >= tokens.size()) {
    errors.push_back(Error{ErrorType::UnexpectedEndOfFile,
                           "Unexpected end of file", tokens.back().line_number,
                           tokens.back().column_number});
    return std::nullopt;
  }

  if (tokens[current].type != expected_type) {
    std::stringstream ss;
    // TODO: read and subsequently forgot about a better strat than static_cast
    ss << "Expected " << static_cast<int>(expected_type) << " but found "
       << static_cast<int>(tokens[current].type);
    errors.push_back(Error{ErrorType::UnexpectedIdentifier, ss.str(),
                           tokens[current].line_number,
                           tokens[current].column_number});
    return std::nullopt;
  }

  return tokens[current++];
}

void Parser::parse_reqline(const std::vector<Token> &tokens, size_t &curr,
                           Request &request) {
  auto method_token = expect_token(tokens, curr, TokenType::Identifier);
  if (!method_token.has_value()) {
    return;
  }

  auto url_token = expect_token(tokens, curr, TokenType::Identifier);
  if (!url_token.has_value()) {
    return;
  }

  request = Request(method_token->value, url_token->value);
}

void Parser::parse_section(const std::vector<Token> &tokens, size_t &curr,
                           Request &request) {
  auto open_bracket_token = expect_token(tokens, curr, TokenType::OpenBracket);
  if (!open_bracket_token.has_value()) {
    return;
  }

  auto section_name_token = expect_token(tokens, curr, TokenType::Identifier);
  if (!section_name_token.has_value()) {
    return;
  }

  std::string section_name = section_name_token->value;

  auto close_bracket_token =
      expect_token(tokens, curr, TokenType::CloseBracket);
  if (!close_bracket_token.has_value()) {
    return;
  }

  if (section_name == "headers") {
    parse_headers(tokens, curr, request);
  } else if (section_name == "body") {
    parse_body(tokens, curr, request);
  } else {
    errors.push_back(Error{ErrorType::UnexpectedIdentifier,
                           "Unrecognized section name: " + section_name,
                           tokens[curr].line_number,
                           tokens[curr].column_number});
  }
}

void Parser::parse_headers(const std::vector<Token> &tokens, size_t &curr,
                           Request &request) {
  while (curr < tokens.size()) {
    auto key_token = expect_token(tokens, curr, TokenType::Identifier);
    if (!key_token.has_value()) {
      return;
    }

    auto colon_token = expect_token(tokens, curr, TokenType::Colon);
    if (!colon_token.has_value()) {
      return;
    }

    // TODO: think about this being a indent vs string literal
    auto value_token = expect_token(tokens, curr, TokenType::StringLiteral);
    if (!value_token.has_value()) {
      return;
    }

    request.add_header(key_token->value + ": " + value_token->value);

    auto newline_token = expect_token(tokens, curr, TokenType::Newline);
    if (!newline_token.has_value()) {
      return;
    }
  }
}

void Parser::parse_body(const std::vector<Token> &tokens, size_t &curr,
                        Request &request) {
  // TODO: just eat the characters until the end of the file or the next section
  while (curr < tokens.size()) {
    auto token = tokens[curr];
    if (token.type == TokenType::EndOfFile) {
      return;
    }
    curr++;
  }

  request.set_json_body("");
}
