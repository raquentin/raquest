#include "assertion_set.hpp"
#include "errors/assertion_error.hpp"
#include <sstream>

std::vector<AssertionError> validate(const CurlResponse &r,
                                     const AssertionSet &as) {

    std::vector<AssertionError> errors;

    int status_code = r.get_status_code();
    if (!as.status_codes.empty() &&
        std::find(as.status_codes.begin(), as.status_codes.end(),
                  status_code) == as.status_codes.end()) {
        std::stringstream ss;
        ss << "Expected one of these status codes: ";
        for (int code : as.status_codes) {
            ss << code << " ";
        }
        ss << "but got " << status_code;
        errors.push_back(AssertionError(
            r.get_file_name(), AssertionError::Type::UnexpectedStatusCode));
    }

    for (const auto &[key, expected_value] : as.headers) {
        if (auto value = r.get_header_value(key)) {
            if (*value != expected_value) {
                std::stringstream ss;
                ss << "Expected header '" << key << "' to have value '"
                   << expected_value << "' but got '" << *value << "'";

                errors.push_back(AssertionError(
                    r.get_file_name(), AssertionError::Type::UnexpectedHeader));
            }
        } else {
            std::stringstream ss;
            ss << "Expected header '" << key << "' to be present but it wasn't";
            errors.push_back(AssertionError(
                r.get_file_name(), AssertionError::Type::MissingHeader));
        }
    }

    for (const auto &[key, pattern] : as.json_fields) {
        auto value = r.get_json_field(key);

        if (!value.has_value()) {

            // "Expected json field '" + key + "' to be present but it wasn't",

            errors.push_back(AssertionError(
                r.get_file_name(), AssertionError::Type::MissingJsonField));
            continue;
        }

        std::string val = value.value();

        if (val.front() == '"' && val.back() == '"') {
            val = val.substr(1, val.size() - 2);
        }

        if (!std::regex_match(val, pattern.get_regex())) {

            //                 "Expected json field '" + key + "' to match
            //                 pattern '" + pattern.get_pattern() + "' but it
            //                 didn't, it was '" + val + "'",

            errors.push_back(AssertionError(
                r.get_file_name(), AssertionError::Type::MissingJsonField));
        }
    }

    return errors;
}
