#include "parser.hpp"
#include <optional>
#include <sstream>

Parser::Parser(const std::string &input)
    : input(input), position(0), line_number(1), column_number(1) {}

std::optional<std::string> Parser::next_line() {
  if (position >= input.size())
    return std::nullopt;

  size_t start = position;
  while (position < input.size() && input[position] != '\n') {
    position++;
    column_number++;
  }

  if (position < input.size() && input[position] == '\n') {
    line_number++;
    column_number = 1;
    position++;
  }

  return input.substr(start, position - start - 1);
}

std::expected<Request, std::vector<Error>> Parser::parse() {
  Request request("", "");
  std::optional<std::string> line;

  while ((line = next_line()).has_value()) {
    if (line->empty() || line->at(0) == '#')
      continue;

    if (line->at(0) == '[') {
      if (*line == "[request]") {
  }
}
