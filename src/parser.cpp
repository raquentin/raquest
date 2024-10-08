#include "parser.hpp"
#include <cstddef>
#include <iostream>
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

    if (!line->empty() && line->at(0) == '[') {
      if (*line == "[request]") {
        parse_request(request);
      } else if (*line == "[headers]") {
        parse_headers(request);
      } else if (line->find("[body") != std::string::npos) {
        parse_body(request);
      } else if (line->find("[assertion") != std::string::npos) {
        parse_assertion(request);
      } else {
        errors.push_back(Error(ErrorType::MalinformedSectionHeader,
                               "Unrecognized section header: " + *line,
                               line_number, column_number));
      }
    }
  }

  if (!errors.empty()) {
    return std::unexpected(errors);
  }

  return request;
}

void Parser::parse_request(Request &request) {
  auto line = next_line();
  if (!line.has_value()) {
    errors.push_back(Error(ErrorType::UnexpectedEndOfFile,
                           "Expected HTTP method and URL", line_number,
                           column_number));
    return;
  }

  std::istringstream iss(*line);
  std::string method, url;
  iss >> method >> url;

  if (method.empty() || url.empty()) {
    errors.push_back(
        Error(ErrorType::ExpectedIdentifier,
              "Invalid request line. Expected HTTP method and URL.",
              line_number, column_number));
  }

  request.set_method(method);
  request.set_url(url);
}

void Parser::parse_headers(Request &request) {
  std::optional<std::string> line;
  while ((line = next_line()).has_value() && !line->empty() &&
         line->at(0) != '[') {
    size_t colon_pos = line->find(':');
    if (colon_pos == std::string::npos) {
      errors.push_back(Error(ErrorType::UnexpectedCharacter,
                             "Expected ':' in header line", line_number,
                             column_number));
      continue;
    }

    std::string key = line->substr(0, colon_pos);
    std::string value = line->substr(colon_pos + 1);
    request.add_header(key, value);
  }
}

void Parser::parse_body(Request &request) {
  std::optional<std::string> line;
  std::stringstream body_content;

  while ((line = next_line()).has_value() && !line->empty() &&
         line->at(0) != '[') {
    body_content << *line << '\n';
  }

  request.set_body(body_content.str());
}

std::string trim(const std::string &str) {
  auto begin = str.begin();
  while (begin != str.end() && std::isspace(*begin)) {
    begin++;
  }

  auto end = str.end();
  do {
    end--;
  } while (std::distance(begin, end) > 0 && std::isspace(*end));

  return std::string(begin, end + 1);
}

void Parser::parse_assertion(Request &request) {
  std::optional<std::string> line;

  Assertions assertions = Assertions::create();

  while ((line = next_line()).has_value() && !line->empty() &&
         line->at(0) != '[') {
    if (line->find("status") != std::string::npos) {
      size_t colon_pos = line->find(':');
      std::string codes = line->substr(colon_pos + 1);
      std::istringstream iss(codes);
      std::vector<int> status_codes;
      int code;
      while (iss >> code) {
        status_codes.push_back(code);
        if (iss.peek() == ',') {
          iss.ignore();
        }
      }
      assertions.status_codes(status_codes);
    } else if (line->find("header") != std::string::npos) {
      size_t equals_pos = line->find('=');
      std::string key = line->substr(7, equals_pos - 7); // Remove "header: "
      std::string value = line->substr(equals_pos + 1);
      assertions.header(key, value);
    } else if (line->find("json_field") != std::string::npos) {
      size_t colon_pos = line->find(':');
      if (colon_pos == std::string::npos) {
        errors.push_back(Error(ErrorType::ExpectedIdentifier,
                               "Invalid json_field syntax", line_number,
                               column_number));
        return;
      }

      size_t field_start = colon_pos + 2; // skip the colon and the space

      size_t value_start = line->find(' ', field_start);
      if (value_start == std::string::npos) {
        errors.push_back(Error(ErrorType::ExpectedIdentifier,
                               "Invalid json_field syntax: missing value",
                               line_number, column_number));
        return;
      }

      std::string field = line->substr(field_start, value_start - field_start);
      std::string value =
          line->substr(value_start + 1); // everything after the first space

      if (field.empty()) {
        errors.push_back(Error(ErrorType::ExpectedIdentifier,
                               "Field name cannot be empty", line_number,
                               column_number));
        return;
      }

      // intelligent type inference (regex, number, boolean, or string match)
      if (value == "true" || value == "false") {
        assertions.json_field(field, value); // boolean field
      } else if (std::regex_match(value, std::regex("^-?\\d+(\\.\\d+)?$"))) {
        assertions.json_field(field, value); // number
      } else {
        // it's a string or regex pattern
        if (value.front() != '^' && value.back() != '$') {
          value = "^" + value + "$"; // regex wrap
        }
        assertions.json_field(field, value);
      }
    }
  }

  request.set_assertions(assertions);
}

std::vector<Error> Parser::get_errors() const { return errors; }
