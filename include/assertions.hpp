#pragma once
#include "regex.hpp"
#include "response.hpp"
#include <string>
#include <vector>

enum class FailedAssertionType {
  UnexpectedStatusCode,
  MissingHeader,
  UnexpectedHeader,
  MissingJsonField,
  UnexpectedJsonField,
};

struct FailedAssertion {
  FailedAssertionType type;
  std::string message;
};

class Assertions {
public:
  static Assertions create();

  Assertions &set_status_codes(const std::vector<int> &expected_codes);
  Assertions &set_header(const std::string &key, const std::string &expected_value);
  Assertions &set_json_field(const std::string &key, const std::string &pattern);

  std::vector<FailedAssertion> validate(const Response &response) const;

private:
  std::vector<int> expected_status_codes;
  std::vector<std::pair<std::string, std::string>> expected_headers;
  std::vector<std::pair<std::string, Regex>> expected_json_fields;

  Assertions();

  std::vector<FailedAssertion>
  validate_status_code(const Response &response) const;
  std::vector<FailedAssertion>
  validate_header(const Response &response) const;
  std::vector<FailedAssertion>
  validate_json_fields(const Response &response) const;
};
