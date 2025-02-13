#pragma once
#include "assertion_set.hpp"
#include "curl_request.hpp"
#include "errors/parser_error.hpp"
#include <expected>
#include <gtest/gtest_prod.h>
#include <optional>
#include <string>

enum class HttpMethod { GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS, UNKNOWN };

/**
 * @brief A class to parse Raquests while collecting tokens and errors.
 */
class Parser {
  public:
    Parser(const std::string &file_name);
    std::expected<std::pair<CurlRequest, std::optional<AssertionSet>>,
                  std::vector<ParserError>>
    parse();

    FRIEND_TEST(ParserTest, NextLineBasic);
    FRIEND_TEST(ParserTest, ParseMinimalValid);
    FRIEND_TEST(ParserTest, ParseMalformedHeader);
    FRIEND_TEST(ParserTest, ConcurrentNextLine);

  private:
    std::string input_;
    const std::string &file_name_;
    size_t position_;
    int line_number_;
    int column_number_;
    std::optional<AssertionSet> assertion_set_ = std::nullopt;

    std::vector<ParserError> errors_;

    std::optional<std::string> next_line();

    void parse_request(CurlRequest &request);
    void parse_headers(CurlRequest &request);
    void parse_body(CurlRequest &request);
    void parse_assertions();
};
