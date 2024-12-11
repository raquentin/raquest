#include "assertions.hpp"
#include "response.hpp"
#include <fmt/color.h>
#include <fmt/core.h>
#include <gtest/gtest.h>

class AssertionsTestFixture : public ::testing::Test {
protected:
  Assertions assertions;

  AssertionsTestFixture() : assertions(Assertions::create()) {}
};

TEST_F(AssertionsTestFixture, SuccessfulValidation) {
  assertions.set_status_codes({200, 201})
      .set_header("Content-Type", "application/json")
      .set_header("Authorization", "Bearer exampletoken12345")
      .set_json_field("title", "^foo$")
      .set_json_field("age", "^30$")
      .set_json_field("isActive", "^true$")
      .set_json_field("description", "^here is the description$");

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

  std::vector<FailedAssertion> results = assertions.validate(response);

  EXPECT_TRUE(results.empty())
      << "Validation should pass but failed with: "
      << (!results.empty() ? results[0].message : "Unknown error");
}

TEST_F(AssertionsTestFixture, UnexpectedStatusCode) {
  assertions.set_status_codes({200, 201});

  Response response;
  response.parse_status_code(404);

  std::vector<FailedAssertion> results = assertions.validate(response);

  ASSERT_EQ(results.size(), 1);
  EXPECT_EQ(results[0].type, FailedAssertionType::UnexpectedStatusCode);
  EXPECT_EQ(results[0].message,
            "Expected one of these status codes: 200 201 but got 404");
}

TEST_F(AssertionsTestFixture, MissingHeader) {
  assertions.set_header("Authorization", "Bearer exampletoken12345");

  Response response;
  response.parse_status_code(200);

  std::vector<FailedAssertion> results = assertions.validate(response);

  ASSERT_EQ(results.size(), 1);
  EXPECT_EQ(results[0].type, FailedAssertionType::MissingHeader);
  EXPECT_EQ(results[0].message,
            "Expected header 'Authorization' to be present but it wasn't");
}

TEST_F(AssertionsTestFixture, UnexpectedHeaderValue) {
  assertions.set_header("Content-Type", "application/json");

  Response response;
  response.parse_status_code(200);
  response.parse_header("Content-Type: text/html");

  std::vector<FailedAssertion> results = assertions.validate(response);

  ASSERT_EQ(results.size(), 1);
  EXPECT_EQ(results[0].type, FailedAssertionType::UnexpectedHeader);
  EXPECT_EQ(results[0].message, "Expected header 'Content-Type' to have value "
                                "'application/json' but got 'text/html'");
}

TEST_F(AssertionsTestFixture, MissingJsonField) {
  assertions.set_json_field("age", "^30$");

  Response response;
  response.parse_status_code(200);
  response.set_body(R"({
          "title": "foo"
      })"); // "age" field is missing

  std::vector<FailedAssertion> results = assertions.validate(response);

  ASSERT_EQ(results.size(), 1);
  EXPECT_EQ(results[0].type, FailedAssertionType::MissingJsonField);
  EXPECT_EQ(results[0].message,
            "Expected json field 'age' to be present but it wasn't");
}

TEST_F(AssertionsTestFixture, JsonFieldPatternMismatch) {
  assertions.set_json_field("title", "^foo$");

  Response response;
  response.set_body(R"({
          "title": "bar"
      })");

  std::vector<FailedAssertion> results = assertions.validate(response);

  ASSERT_EQ(results.size(), 1);
  EXPECT_EQ(results[0].type, FailedAssertionType::UnexpectedJsonField);
  EXPECT_EQ(results[0].message, "Expected json field 'title' to match pattern "
                                "'^foo$' but it didn't, it was 'bar'");
}
