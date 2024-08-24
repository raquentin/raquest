#pragma once

#include <string>
#include <expected>
#include <vector>
#include "error.hpp"

enum class TokenType {
    Method,
    Url,
    HeaderSection,
    BodySection,
    BodyType,
    HeaderKey,
    HeaderValue,
    BodyContent,
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
    std::expected<Token, ParseError> tokenize();

private:
    std::string input;
    size_t position;
    int line_number;

    std::vector<ParseError> errors;

    void skip_whitespace_and_comments();
    std::optional<Token> tokenize_section();
    std::optional<Token> tokenize_method();
    std::optional<Token> tokenize_headers();
    std::optional<Token> tokenize_body();
};
