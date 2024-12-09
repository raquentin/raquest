#include "assertions.hpp"
#include "response.hpp"
#include <gtest/gtest.h>

class AssertionsTestFixture : public ::testing::Test {
protected:
  Assertions assertions;

  AssertionsTestFixture() : assertions(Assertions::create()) {}
};

TEST_F(AssertionsTestFixture, SuccessfulValidation) {
  assertions.status_codes({200, 201})
      .header("Content-Type", "application/json")
      .header("Authorization", "Bearer exampletoken12345")
      .json_field("title", "^foo$")
      .json_field("age", "^30$")
      .json_field("isActive", "^true$")
      .json_field("description", "^here is the description$");

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

  std::optional<FailedAssertion> result = assertions.validate(response);

  EXPECT_FALSE(result.has_value())
      << "Validation should pass but failed with: " << result->message;
}

TEST_F(AssertionsTestFixture, UnexpectedStatusCode) {
  assertions.status_codes({200, 201});

  Response response;
  response.parse_status_code(404);

  std::optional<FailedAssertion> result = assertions.validate(response);

  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->type, FailedAssertionType::UnexpectedStatusCode);
  EXPECT_EQ(result->message,
            "Expected one of these status codes: 200 201 but got 404");
}

TEST_F(AssertionsTestFixture, MissingHeader) {
  assertions.header("Authorization", "Bearer exampletoken12345");

  Response response;
  response.parse_status_code(200);

  std::optional<FailedAssertion> result = assertions.validate(response);

  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->type, FailedAssertionType::MissingHeader);
  EXPECT_EQ(result->message,
            "Expected header 'Authorization' to be present but it wasn't");
}

TEST_F(AssertionsTestFixture, UnexpectedHeaderValue) {
  assertions.header("Content-Type", "application/json");

  Response response;
  response.parse_status_code(200);
  response.parse_header("Content-Type: text/html");

  std::optional<FailedAssertion> result = assertions.validate(response);

  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->type, FailedAssertionType::UnexpectedHeader);
  EXPECT_EQ(result->message, "Expected header 'Content-Type' to have value "
                             "'application/json' but got 'text/html'");
}

TEST_F(AssertionsTestFixture, MissingJsonField) {
  assertions.json_field("age", "^30$");

  Response response;
  response.parse_status_code(200);
  response.set_body(R"({
        "title": "foo",
    })"); // "age" field is missing

  std::optional<FailedAssertion> result = assertions.validate(response);

  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->type, FailedAssertionType::MissingJsonField);
  EXPECT_EQ(result->message,
            "Expected json field 'age' to be present but it wasn't");
}

TEST_F(AssertionsTestFixture, JsonFieldPatternMismatch) {
  assertions.json_field("title", "^foo$");

  Response response;
  response.set_body(R"({
        "title": "bar"
    })");

  std::optional<FailedAssertion> result = assertions.validate(response);

  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->type, FailedAssertionType::UnexpectedJsonField);
  EXPECT_EQ(result->message, "Expected json field 'title' to match pattern '^foo$' but it didn't, it was 'bar'");
}
