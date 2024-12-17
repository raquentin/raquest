#include "parser.hpp"
#include "assert.hpp"
#include "errors/error.hpp"
#include "errors/error_manager.hpp"
#include "errors/parser_error.hpp"
#include "errors/runtime_error.hpp"
#include "printer.hpp"
#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <utility>

Parser::Parser(const std::string &file_name, ErrorManager &error_manager)
    : file_name(file_name), error_manager(error_manager), position(0),
      line_number(1), column_number(1) {

  std::ifstream file(file_name, std::ios::in | std::ios::binary);

  if (!file) {
    error_manager.add_error(std::make_shared<RuntimeError>(
        file_name, "failed to open file: " + file_name, ErrorSeverity::Error,
        RuntimeErrorType::Internal));
  }

  input.assign((std::istreambuf_iterator<char>(file)),
               std::istreambuf_iterator<char>());

  file.close();
}

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

std::optional<std::shared_ptr<Request>> Parser::parse() {
  Request request(file_name, "", "", error_manager);
  std::optional<std::string> line;

  print_compiling(file_name);

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
        request.set_assertion_set(parse_assertions());
      } else {
        // TODO: generalize this error
        if (line->find("[request") != std::string::npos) {
          const Hint hint =
              Hint{std::pair<int, int>(3, 4), "missing closing bracket ^"};
          const MalformedSectionHeaderInfo &info = {"missing closing bracket",
                                                    line_number, *line,
                                                    std::optional<Hint>(hint)};
          auto error = std::make_shared<ParserError>(
              ParserError(file_name, info, ErrorSeverity::Error));
          error_manager.add_error(error);
        }
      }
    }
  }

  // Checking size and returning here so that we can catch multiple parser
  // errors, not just returning after the first one.
  if (error_manager.get_errors().empty()) {
    return std::make_shared<Request>(request);
  }
  return std::nullopt;
}

void Parser::parse_request(Request &request) {
  auto line = next_line();
  if (!line.has_value()) {
    // errors.push_back(Error(ErrorType::UnexpectedEndOfFile,
    //                        "Expected HTTP method and URL", line_number,
    //                        column_number));
    return;
  }

  std::istringstream iss(*line);
  std::string method, url;
  iss >> method >> url;

  if (method.empty() || url.empty()) {
    // errors.push_back(
    //     Error(ErrorType::ExpectedIdentifier,
    //           "Invalid request line. Expected HTTP method and URL.",
    //           line_number, column_number));
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
      // errors.push_back(Error(ErrorType::UnexpectedCharacter,
      //                        "Expected ':' in header line", line_number,
      //                        column_number));
      continue;
    }

    std::string key = line->substr(0, colon_pos);
    std::string value = line->substr(colon_pos + 1);
    request.add_header(key, value);
  }
}

// TODO: know this is json
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

AssertionSet Parser::parse_assertions() {
  std::optional<std::string> line;

  AssertionSet as{};

  while ((line = next_line()).has_value() && !line->empty() &&
         line->at(0) != '[') {
    if (line->find("status") != std::string::npos) {
      size_t colon_pos = line->find(':');
      std::string codes = line->substr(colon_pos + 1);
      std::istringstream iss(codes);
      std::vector<int> status_codes;
      int code;
      while (iss >> code) {
        as.expected_status_codes.push_back(code);
        if (iss.peek() == ',') {
          iss.ignore();
        }
      }
    } else if (line->find("header") != std::string::npos) {
      size_t equals_pos = line->find('=');
      std::string key = line->substr(7, equals_pos - 7); // Remove "header: "
      std::string value = line->substr(equals_pos + 1);
      as.expected_headers.push_back(std::pair(key, value));
    } else if (line->find("json_field") != std::string::npos) {
      size_t colon_pos = line->find(':');
      if (colon_pos == std::string::npos) {
        // errors.push_back(Error(ErrorType::ExpectedIdentifier,
        //                        "Invalid json_field syntax", line_number,
        //                        column_number));
      }

      size_t field_start = colon_pos + 2; // skip the colon and the space

      size_t value_start = line->find(' ', field_start);
      if (value_start == std::string::npos) {
        // errors.push_back(Error(ErrorType::ExpectedIdentifier,
        //                        "Invalid json_field syntax: missing value",
        //                        line_number, column_number));
      }

      std::string field = line->substr(field_start, value_start - field_start);
      std::string value =
          line->substr(value_start + 1); // everything after the first space

      if (field.empty()) {
        // errors.push_back(Error(ErrorType::ExpectedIdentifier,
        //                        "Field name cannot be empty", line_number,
        //                        column_number));
      }

      // intelligent type inference (regex, number, boolean, or string match)
      // TODO: this logic in general sucks
      if (value == "true" || value == "false") {
        as.expected_json_fields.push_back(std::pair(field, value));
      } else if (std::regex_match(value, std::regex("^-?\\d+(\\.\\d+)?$"))) {
        as.expected_json_fields.push_back(std::pair(field, value));
      } else {
        // it's a string or regex pattern
        // TODO: this logic is not correct
        // should really just have a regex identifier like R"" or something
        if (value.front() != '^' && value.back() != '$') {
          value = "^" + value + "$"; // regex wrap
        }
        as.expected_json_fields.push_back(std::pair(field, value));
      }
    }
  }

  return as;
}
