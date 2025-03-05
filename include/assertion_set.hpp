#pragma once

#include "curl_response.hpp"
#include "errors/assertion_error.hpp"
#include "regex.hpp"
#include <string>
#include <vector>

struct StatusCodeAssertion {
    int line_number;
    int code;
};

struct HeaderAssertion {
    int line_number;
    std::string key;
    std::string value;
};

struct JsonFieldAssertion {
    int line_number;
    std::string key;
    Regex pattern;
};

struct AssertionSet {
    std::vector<StatusCodeAssertion> status_codes;
    std::vector<HeaderAssertion> headers;
    std::vector<JsonFieldAssertion> json_fields;
};

std::vector<AssertionError> validate(const CurlResponse &response,
                                     const AssertionSet &assertion_set);
