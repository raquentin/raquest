#pragma once

#include "curl_response.hpp"
#include "errors/assertion_error.hpp"
#include "regex.hpp"
#include <string>
#include <vector>

struct AssertionSet {
    std::vector<int> status_codes;
    std::vector<std::pair<std::string, std::string>> headers;
    std::vector<std::pair<std::string, Regex>> json_fields;
};

std::vector<AssertionError> validate(const CurlResponse &response,
                                     const AssertionSet &assertion_set);
