#include "error.hpp"
#include <fmt/color.h>
#include <fmt/core.h>

Error::Error(ErrorType type, const std::string &line_content, int line_number,
             int column_number)
    : type(type), line_content(line_content), line_number(line_number),
      column_number(column_number) {}

std::string Error::pretty_sprint() const { return line_content; }

std::ostream &operator<<(std::ostream &os, const Error &error) {
  return os << error.pretty_sprint() << std::endl;
}
