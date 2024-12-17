#pragma once

#include "errors/error_manager.hpp"
#include "regex.hpp"
#include "response.hpp"
#include <string>
#include <vector>

struct AssertionSet {
  // TODO: make this a set
  std::vector<int> expected_status_codes;
  std::vector<std::pair<std::string, std::string>> expected_headers;
  std::vector<std::pair<std::string, Regex>> expected_json_fields;
};

void validate(const Response &response, const AssertionSet &assertion_set,
              ErrorManager &error_manager);
