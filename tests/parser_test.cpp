#include <gtest/gtest.h>
#include <fstream>
#include "parser.hpp"

using namespace std;

class ParserTest : public ::testing::Test {};

TEST_F(ParserTest, ParseFile_CorrectlyParsesHeaders) {
    // Sample request content
    string file_content = 
        "GET /example\n"
        "Host: example.com\n"
        "Content-Type: application/json\n"
        "\n"
        "{\"key\": \"value\"}\n";

    // Write content to a temporary file
    ofstream test_file("test_request.txt");
    test_file << file_content;
    test_file.close();

    // Parse the file
    Parser parser;
    Request request = parser.parse_file("test_request.txt");

    // Validate the parsed data
    EXPECT_EQ(request.get_method(), "GET");
    EXPECT_EQ(request.get_url(), "/example");
    cout << request.get_headers()[2] << endl;
    ASSERT_EQ(request.get_headers().size(), 2);
    EXPECT_EQ(request.get_headers()[0], "Host: example.com");
    EXPECT_EQ(request.get_headers()[1], "Content-Type: application/json");
    EXPECT_EQ(request.get_json_body(), "{\"key\": \"value\"}\n");
}
