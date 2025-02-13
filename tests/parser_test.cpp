#include "parser.hpp"
#include <fstream>
#include <gtest/gtest.h>
#include <thread>

/**
 * @brief Helper to mock a file.
 */
static void writeTempFile(const std::string &filename,
                          const std::string &content) {
    std::ofstream out(filename, std::ios::binary);
    out << content;
    out.close();
}

/**
 * @brief Dumb test for Parser::next_line.
 */
TEST(ParserTest, NextLineBasic) {
    const std::string testFile = "test_input.txt";
    writeTempFile(testFile, "line1\nline2\nline3\n");

    Parser parser(testFile);

    auto line1 = parser.next_line();
    ASSERT_TRUE(line1.has_value());
    EXPECT_EQ(line1.value(), "line1");

    auto line2 = parser.next_line();
    ASSERT_TRUE(line2.has_value());
    EXPECT_EQ(line2.value(), "line2");

    auto line3 = parser.next_line();
    ASSERT_TRUE(line3.has_value());
    EXPECT_EQ(line3.value(), "line3");

    auto noLine = parser.next_line();
    EXPECT_FALSE(noLine.has_value());
}

/**
 * @brief Very basic but section-complete should parse.
 */
TEST(ParserTest, ParseMinimalValid) {
    const std::string testFile = "test_valid.txt";
    writeTempFile(testFile,
                  R"([request]
GET https://example.com
[headers]
Content-Type: application/json
[body]
{"key": "value"}
[assertions]
status: 200, 201
)");

    Parser parser(testFile);

    auto result = parser.parse();
    EXPECT_TRUE(result.has_value()) << "Parser returned unexpected errors. "
                                       "Possibly see result.error().size()";

    if (result.has_value()) {
        auto [request, assertionsOpt] = result.value();

        EXPECT_EQ(request.method_, "GET");
        EXPECT_EQ(request.url_, "https://example.com");
        ASSERT_TRUE(assertionsOpt.has_value());
        auto &as = assertionsOpt.value();

        ASSERT_FALSE(as.status_codes.empty());
        EXPECT_EQ(as.status_codes[0], 200);
    }
}

/**
 * @brief Malformed header should error.
 */
TEST(ParserTest, ParseMalformedHeader) {
    const std::string testFile = "test_malformed_header.txt";
    writeTempFile(testFile,
                  R"([request]
GET https://example.org
[headers]
Content-Type: text/plain
BadHeaderLineWithoutColon # missing colon here
[body]
Some content
)");

    Parser parser(testFile);
    auto result = parser.parse();
    EXPECT_FALSE(result.has_value());

    if (!result.has_value()) {
        auto &errors = result.error();
        EXPECT_FALSE(errors.empty());
    }
}

/**
 * @brief Parse assertions.
 */
TEST(ParserTest, ParseAssertions) {
    const std::string testFile = "test_assertions.txt";
    writeTempFile(testFile,
                  R"([assertions]
status: 200, 404
header=Content-Type: application/json
json_field: userId 123
json_field: userName bob
json_field: isAdmin true
)");

    Parser parser(testFile);
    auto result = parser.parse();
    EXPECT_TRUE(result.has_value()) << "Expected to parse without errors.";

    if (result.has_value()) {
        auto [req, assertionsOpt] = result.value();
        ASSERT_TRUE(assertionsOpt.has_value());
        auto &as = assertionsOpt.value();

        EXPECT_EQ(as.status_codes.size(), 2);
        EXPECT_EQ(as.status_codes[0], 200);
        EXPECT_EQ(as.status_codes[1], 404);

        EXPECT_FALSE(as.headers.empty());
        EXPECT_EQ(as.headers[0].first, "Content-Type");
        EXPECT_EQ(as.headers[0].second, " application/json");

        EXPECT_EQ(as.json_fields.size(), 3u);
        // TODO: check the fields
    }
}

/**
 * @brief Memory leak test with a huge file.
 */
TEST(ParserTest, MemoryStressLargeFile) {
    const std::string testFile = "test_large.txt";
    {
        std::ofstream out(testFile, std::ios::binary);
        for (int i = 0; i < 10000; ++i) {
            out << "[request]\n";
            out << "GET https://example.com/page" << i << "\n";
        }
        out.close();
    }

    Parser parser(testFile);
    auto result = parser.parse();
    SUCCEED() << "Parser completed parse on huge input without crashing.";
}
