#pragma once
#include <iostream>
#include <regex>
#include <string>

class Regex {
public:
  Regex(const std::string &pattern) : pattern_str(pattern), regex(pattern) {}

  const std::regex &get_regex() const { return regex; }
  const std::string &get_pattern() const { return pattern_str; }

  friend std::ostream &operator<<(std::ostream &os, const Regex &regex) {
    os << regex.pattern_str;
    return os;
  }

private:
  std::string pattern_str;
  std::regex regex;
};
