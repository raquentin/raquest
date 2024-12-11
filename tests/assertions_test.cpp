#include "assertions.hpp"
#include "response.hpp"
#include <gtest/gtest.h>
#include <fmt/color.h>
#include <fmt/core.h>

class AssertionsTestFixture : public ::testing::Test {
protected:
    Assertions assertions;

    AssertionsTestFixture() : assertions(Assertions::create()) {}
};

// Test Case: Successful Validation (No Errors Expected)
TEST_F(AssertionsTestFixture, SuccessfulValidation) {
    // Set up expected assertions
    assertions.set_status_codes({200, 201})
        .set_header("Content-Type", "application/json")
        .set_header("Authorization", "Bearer exampletoken12345")
        .set_json_field("title", "^foo$")
        .set_json_field("age", "^30$")
        .set_json_field("isActive", "^true$")
        .set_json_field("description", "^here is the description$");

    // Create and set up a successful Response object
    Response response;
    response.parse_status_code(201);
    response.parse_header("Content-Type: application/json");
    response.parse_header("Authorization: Bearer exampletoken12345");
    response.set_body(R"({
          "title": "foo",
          "age": "30",
          "isActive": "true",
          "description": "here is the description"
      })");

    // Validate the response
    std::vector<FailedAssertion> results = assertions.validate(response);

    // Expect no errors
    EXPECT_TRUE(results.empty()) 
        << "Validation should pass but failed with: " 
        << (!results.empty() ? results[0].message : "Unknown error");
}

// Test Case: Unexpected Status Code
TEST_F(AssertionsTestFixture, UnexpectedStatusCode) {
    // Set up expected status codes
    assertions.set_status_codes({200, 201});

    // Create a Response object with an unexpected status code
    Response response;
    response.parse_status_code(404);

    // Validate the response
    std::vector<FailedAssertion> results = assertions.validate(response);

    // Expect one specific error
    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].type, FailedAssertionType::UnexpectedStatusCode);
    EXPECT_EQ(results[0].message,
              "Expected one of these status codes: 200 201 but got 404");
}

// Test Case: Missing Header
TEST_F(AssertionsTestFixture, MissingHeader) {
    // Set up expected header
    assertions.set_header("Authorization", "Bearer exampletoken12345");

    // Create a Response object missing the Authorization header
    Response response;
    response.parse_status_code(200);

    // Validate the response
    std::vector<FailedAssertion> results = assertions.validate(response);

    // Expect one specific error
    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].type, FailedAssertionType::MissingHeader);
    EXPECT_EQ(results[0].message,
              "Expected header 'Authorization' to be present but it wasn't");
}

// Test Case: Unexpected Header Value
TEST_F(AssertionsTestFixture, UnexpectedHeaderValue) {
    // Set up expected header
    assertions.set_header("Content-Type", "application/json");

    // Create a Response object with an unexpected Content-Type header
    Response response;
    response.parse_status_code(200);
    response.parse_header("Content-Type: text/html");

    // Validate the response
    std::vector<FailedAssertion> results = assertions.validate(response);

    // Expect one specific error
    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].type, FailedAssertionType::UnexpectedHeader);
    EXPECT_EQ(results[0].message, "Expected header 'Content-Type' to have value 'application/json' but got 'text/html'");
}

// Test Case: Missing JSON Field
TEST_F(AssertionsTestFixture, MissingJsonField) {
    // Set up expected JSON field
    assertions.set_json_field("age", "^30$");

    // Create a Response object missing the 'age' JSON field
    Response response;
    response.parse_status_code(200);
    response.set_body(R"({
          "title": "foo"
      })"); // "age" field is missing

    // Validate the response
    std::vector<FailedAssertion> results = assertions.validate(response);

    // Expect one specific error
    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].type, FailedAssertionType::MissingJsonField);
    EXPECT_EQ(results[0].message,
              "Expected json field 'age' to be present but it wasn't");
}

// Test Case: JSON Field Pattern Mismatch
TEST_F(AssertionsTestFixture, JsonFieldPatternMismatch) {
    // Set up expected JSON field pattern
    assertions.set_json_field("title", "^foo$");

    // Create a Response object with a mismatched 'title' JSON field
    Response response;
    response.set_body(R"({
          "title": "bar"
      })");

    // Validate the response
    std::vector<FailedAssertion> results = assertions.validate(response);

    // Expect one specific error
    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].type, FailedAssertionType::UnexpectedJsonField);
    EXPECT_EQ(results[0].message, "Expected json field 'title' to match pattern '^foo$' but it didn't, it was 'bar'");
}

