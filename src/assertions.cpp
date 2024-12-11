#include "assertions.hpp"
#include <iostream>
#include <sstream>

Assertions Assertions::create() { return Assertions(); }

Assertions::Assertions() {}

Assertions &Assertions::set_status_codes(const std::vector<int> &expected_codes) {
  this->expected_status_codes = expected_codes;
  return *this;
}

Assertions &Assertions::set_header(const std::string &key,
                               const std::string &expected_value) {
  this->expected_headers.emplace_back(key, expected_value);
  return *this;
}

Assertions &Assertions::set_json_field(const std::string &key,
                                   const std::string &pattern) {
  this->expected_json_fields.emplace_back(key, Regex(pattern));
  return *this;
}

std::vector<FailedAssertion>
Assertions::validate(const Response &response) const {
  std::vector<FailedAssertion> failures;
  
  auto status_failures = validate_status_code(response);
  failures.insert(failures.end(), status_failures.begin(), status_failures.end());
  
  auto header_failures = validate_header(response);
  failures.insert(failures.end(), header_failures.begin(), header_failures.end());
  
  auto json_failures = validate_json_fields(response);
  failures.insert(failures.end(), json_failures.begin(), json_failures.end());
  
  return failures;
}

std::vector<FailedAssertion>
Assertions::validate_status_code(const Response &response) const {
  std::vector<FailedAssertion> failures;
  int status_code = response.get_status_code();
  if (!expected_status_codes.empty() &&
      std::find(expected_status_codes.begin(), expected_status_codes.end(),
                status_code) == expected_status_codes.end()) {
    std::stringstream ss;
    ss << "Expected one of these status codes: ";
    for (int code : expected_status_codes) {
      ss << code << " ";
    }
    ss << "but got " << status_code;
    failures.emplace_back(FailedAssertionType::UnexpectedStatusCode, ss.str());
  }
  return failures;
}

std::vector<FailedAssertion>
Assertions::validate_header(const Response &response) const {
  std::vector<FailedAssertion> failures;
  for (const auto &[key, expected_value] : expected_headers) {
    if (auto value = response.get_header_value(key)) {
      if (*value != expected_value) {
        std::stringstream ss;
        ss << "Expected header '" << key << "' to have value '"
           << expected_value << "' but got '" << *value << "'";
        failures.emplace_back(FailedAssertionType::UnexpectedHeader, ss.str());
      }
    } else {
      std::stringstream ss;
      ss << "Expected header '" << key << "' to be present but it wasn't";
      failures.emplace_back(FailedAssertionType::MissingHeader, ss.str());
    }
  }
  return failures;
}

std::vector<FailedAssertion>
Assertions::validate_json_fields(const Response &response) const {
  std::vector<FailedAssertion> failures;
  for (const auto &[key, pattern] : expected_json_fields) {
    auto value = response.get_json_field(key);

    if (!value.has_value()) {
      failures.emplace_back(FailedAssertionType::MissingJsonField,
                          "Expected json field '" + key +
                              "' to be present but it wasn't");
      continue;
    }

    std::string val = value.value();

    if (val.front() == '"' && val.back() == '"') {
      val = val.substr(1, val.size() - 2);
    }

    if (!std::regex_match(val, pattern.get_regex())) {
      failures.emplace_back(
          FailedAssertionType::UnexpectedJsonField,
          "Expected json field '" + key + "' to match pattern '" +
              pattern.get_pattern() + "' but it didn't, it was '" + val + "'");
    }
  }
  return failures;
}
