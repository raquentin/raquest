#include "assertion_set.hpp"
#include "errors/assertion_error.hpp"
#include <sstream>

std::vector<AssertionError> validate(const CurlResponse &r,
                                     const AssertionSet &as) {

    std::vector<AssertionError> errors;

    // validate status codes
    int status_code = r.get_status_code();
    if (!as.status_codes.empty()) {

        for (const StatusCodeAssertion &sc : as.status_codes) {
            if (sc.code == status_code)
                goto SKIP_CODES;
        }

        std::stringstream ss;
        ss << "Expected one of these status codes: ";
        for (const auto &sca : as.status_codes) {
            ss << sca.code << " ";
        }
        ss << "but got " << status_code;
        /*errors.emplace_back(AssertionError(*/
        /*    r.get_file_name(), AssertionError::Type::UnexpectedStatusCode));*/
    }

SKIP_CODES:
    for (const auto &[ln, key, expected_value] : as.headers) {
        if (auto value = r.get_header_value(key)) {
            if (*value != expected_value) {
                std::stringstream ss;
                ss << "Expected header '" << key << "' to have value '"
                   << expected_value << "' but got '" << *value << "'";

                /*errors.emplace_back(AssertionError(*/
                /*    r.get_file_name(),
                 * AssertionError::Type::UnexpectedHeader));*/
            }
        } else {
            std::stringstream ss;
            ss << "Expected header '" << key << "' to be present but it wasn't";
            /*errors.emplace_back(AssertionError(*/
            /*    r.get_file_name(), AssertionError::Type::MissingHeader));*/
        }
    }

    for (const auto &[ln, key, pattern] : as.json_fields) {
        auto value = r.get_json_field(key);

        // error that key does not exist
        if (!value.has_value()) {
            // TODO:
        }

        std::string val = value.value();

        if (val.front() == '"' && val.back() == '"') {
            val = val.substr(1, val.size() - 2);
        }

        if (!std::regex_match(val, pattern.get_regex())) {
            // find index of key in text
            size_t colon_pos = 3;

            auto emph_range = std::pair(colon_pos, colon_pos + key.length());

            Hint hint =
                Hint{emph_range, "this key was not found in the respone"};

            MissingJsonField info{ln, "code snippet", hint};

            errors.push_back(AssertionError(r.get_file_name(), info));
            continue;
        }
    }

    return errors;
}
