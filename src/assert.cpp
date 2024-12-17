#include "assert.hpp"
#include "errors/assertion_error.hpp"
#include <memory>

void validate(const Response &r, const AssertionSet &as, ErrorManager &em) {
  int status_code = r.get_status_code();
  if (!as.expected_status_codes.empty() &&
      std::find(as.expected_status_codes.begin(),
                as.expected_status_codes.end(),
                status_code) == as.expected_status_codes.end()) {
    std::stringstream ss;
    ss << "Expected one of these status codes: ";
    for (int code : as.expected_status_codes) {
      ss << code << " ";
    }
    ss << "but got " << status_code;
    em.add_error(std::make_unique<AssertionError>(
        r.get_file_name(), ss.str(), AssertionErrorType::UnexpectedStatusCode));
  }

  for (const auto &[key, expected_value] : as.expected_headers) {
    if (auto value = r.get_header_value(key)) {
      if (*value != expected_value) {
        std::stringstream ss;
        ss << "Expected header '" << key << "' to have value '"
           << expected_value << "' but got '" << *value << "'";

        em.add_error(std::make_unique<AssertionError>(
            r.get_file_name(), ss.str(), AssertionErrorType::UnexpectedHeader));
      }
    } else {
      std::stringstream ss;
      ss << "Expected header '" << key << "' to be present but it wasn't";
      em.add_error(std::make_unique<AssertionError>(
          r.get_file_name(), ss.str(), AssertionErrorType::MissingHeader));
    }
  }

  for (const auto &[key, pattern] : as.expected_json_fields) {
    auto value = r.get_json_field(key);

    if (!value.has_value()) {

      em.add_error(std::make_unique<AssertionError>(
          r.get_file_name(),
          "Expected json field '" + key + "' to be present but it wasn't",
          AssertionErrorType::MissingJsonField));
      continue;
    }

    std::string val = value.value();

    if (val.front() == '"' && val.back() == '"') {
      val = val.substr(1, val.size() - 2);
    }

    if (!std::regex_match(val, pattern.get_regex())) {

      em.add_error(std::make_unique<AssertionError>(
          r.get_file_name(),
          "Expected json field '" + key + "' to match pattern '" +
              pattern.get_pattern() + "' but it didn't, it was '" + val + "'",
          AssertionErrorType::MissingJsonField));
    }
  }
}
