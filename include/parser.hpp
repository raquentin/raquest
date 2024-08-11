#pragma once

#include "request.hpp"
#include <string>

class Parser {
public:
  static Request parse_file(const std::string &file_path);
};
