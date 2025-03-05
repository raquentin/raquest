#include "parser.hpp"
#include "assertion_set.hpp"
#include "errors/parser_error.hpp"
#include "printer.hpp"
#include <cstddef>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <utility>

Parser::Parser(const std::string &file_name)
    : file_name_(file_name), position_(0), line_number_(1), column_number_(1) {

    std::ifstream file(file_name, std::ios::in | std::ios::binary);

    if (!file) {
        // TODO: file error
    }

    input_.assign((std::istreambuf_iterator<char>(file)),
                  std::istreambuf_iterator<char>());

    file.close();
}

std::optional<std::string> Parser::next_line() {
    if (position_ >= input_.size())
        return std::nullopt;

    size_t start = position_;
    while (position_ < input_.size() && input_[position_] != '\n') {
        position_++;
        column_number_++;
    }

    if (position_ < input_.size() && input_[position_] == '\n') {
        line_number_++;
        column_number_ = 1;
        position_++;
    }

    std::size_t len = (position_ > start && input_[position_ - 1] == '\n')
                          ? (position_ - start - 1)
                          : (position_ - start);

    return input_.substr(start, len);
}

std::expected<std::pair<CurlRequest, std::optional<AssertionSet>>,
              std::vector<ParserError>>
Parser::parse() {

    printer().compiling(file_name_);

    std::optional<std::string> line;
    CurlRequest request(file_name_);

    while ((line = next_line()).has_value()) {
        // skip empty
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
                parse_assertions();
            } else {
                // TODO: generalize this error
                if (line->find("[request") != std::string::npos) {
                    const Hint hint = Hint{std::pair<int, int>(3, 4),
                                           "add closing bracket ^"};
                    MalformedSectionHeader info = {"missing closing bracket",
                                                   line_number_, *line,
                                                   std::optional<Hint>(hint)};
                    errors_.push_back(ParserError(file_name_, info));
                }
            }
        }
    }

    if (!errors_.empty())
        return std::unexpected(errors_);

    return std::make_pair(request, assertion_set_);
}

void Parser::parse_request(CurlRequest &request) {
    auto line = next_line();
    if (!line.has_value()) {
        // TODO: error
        return;
    }

    std::istringstream iss(*line);
    std::string method, url;
    iss >> method >> url;

    if (method.empty() || url.empty()) {
        // TODO: error
        return;
    }

    request.method_ = method;
    request.url_ = url;
}

void Parser::parse_headers(CurlRequest &request) {
    std::optional<std::string> line;

    while ((line = next_line()).has_value() && !line->empty() &&
           line->at(0) != '[') {
        size_t colon_pos = line->find(':');
        if (colon_pos == std::string::npos) {
            int space_pos = static_cast<int>(line->find(' '));
            std::pair<int, int> emph_range =
                std::pair(space_pos, space_pos + 1);
            Hint hint = Hint{emph_range, "add colon"};
            ExpectedColonInHeaderAssignment info{line_number_, *line, hint};
            errors_.push_back(ParserError(file_name_, info));
            continue;
        }

        std::string key = line->substr(0, colon_pos);
        std::string value = line->substr(colon_pos + 1);

        request.headers_.emplace_back(key + ": " + value);
    }
}

// TODO: know this is json
void Parser::parse_body(CurlRequest &request) {
    std::optional<std::string> line;
    std::stringstream body_content;

    while ((line = next_line()).has_value() && !line->empty() &&
           line->at(0) != '[') {
        body_content << *line << '\n';
    }

    // TODO: validate json

    request.body_ = body_content.str();
}

std::string trim(const std::string &str) {
    auto begin = str.begin();
    while (begin != str.end() &&
           std::isspace(static_cast<unsigned char>(*begin))) {
        begin++;
    }

    auto end = str.end();
    while (std::distance(begin, end) > 0 &&
           std::isspace(static_cast<unsigned char>(*(end - 1))))
        end--;

    return std::string(begin, end + 1);
}

void Parser::parse_assertions() {
    if (!assertion_set_.has_value()) {
        assertion_set_.emplace();
    }

    std::optional<std::string> line;

    while ((line = next_line()).has_value() && !line->empty() &&
           line->at(0) != '[') {
        if (line->find("status") != std::string::npos) {
            size_t colon_pos = line->find(':');
            if (colon_pos == std::string::npos) {
                // TODO: error
                continue;
            }

            std::string codes = line->substr(colon_pos + 1);
            std::istringstream iss(codes);
            std::vector<int> status_codes;
            int code;
            while (iss >> code) {
                assertion_set_->status_codes.emplace_back(line_number_, code);
                if (iss.peek() == ',') {
                    iss.ignore();
                }
            }
        } else if (line->find("header") != std::string::npos) {
            size_t equals_pos = line->find('=');
            if (equals_pos == std::string::npos) {
                // TODO: error
                continue;
            }

            std::string kv = line->substr(equals_pos + 1);
            // naive parse: split on ':'
            auto colon_pos = kv.find(':');
            if (colon_pos == std::string::npos) {
                // TODO: error
                continue;
            }

            std::string key = kv.substr(0, colon_pos);
            std::string value = kv.substr(colon_pos + 1);

            assertion_set_->headers.emplace_back(line_number_, key, value);
        } else if (line->find("json_field") != std::string::npos) {
            size_t colon_pos = line->find(':');
            if (colon_pos == std::string::npos) {
                // errors.push_back(Error(ErrorType::ExpectedIdentifier,
                //                        "Invalid json_field syntax",
                //                        line_number, column_number));
                //

                continue;
            }

            size_t field_start = colon_pos + 2; // skip the colon and the space
            size_t value_start = line->find(' ', field_start);
            if (value_start == std::string::npos) {
                // errors.push_back(Error(ErrorType::ExpectedIdentifier,
                //                        "Invalid json_field syntax: missing
                //                        value", line_number, column_number));
                //

                continue;
            }

            std::string field =
                line->substr(field_start, value_start - field_start);
            std::string value = line->substr(
                value_start + 1); // everything after the first space

            if (field.empty()) {
                // errors.push_back(Error(ErrorType::ExpectedIdentifier,
                //                        "Field name cannot be empty",
                //                        line_number, column_number));

                continue;
            }

            // poor man's type inference
            if (value == "true" || value == "false") {
                assertion_set_->json_fields.emplace_back(line_number_, field,
                                                         value);
            } else if (std::regex_match(value,
                                        std::regex("^-?\\d+(\\.\\d+)?$"))) {
                assertion_set_->json_fields.emplace_back(line_number_, field,
                                                         value);
            } else {
                // it's a string or regex pattern
                // TODO: this logic is not correct
                // should really just have a regex identifier like R"" or
                // something
                if (value.front() != '^' && value.back() != '$') {
                    value = "^" + value + "$"; // regex wrap
                }
                assertion_set_->json_fields.emplace_back(line_number_, field,
                                                         value);
            }
        }
    }
}
