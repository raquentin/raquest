#pragma once
#include "error.hpp"
#include <optional>
#include <string>
#include <variant>

enum class ParserErrorType {
  MalformedSectionHeader,
  ExpectedColonInHeaderAssignment,
};

struct MalformedSectionHeaderInfo {
  std::string section_name;
  int line_number;
  std::string snippet;
  std::optional<Hint> hint;
};

struct ExpectedColonInHeaderAssignmentInfo {
  int line_number;
  std::string snippet;
  Hint hint;
};

class ParserError : public Error {
public:
  ParserError(const std::string &file_name,
              const MalformedSectionHeaderInfo &info,
              const ErrorSeverity severity);

  ParserError(const std::string &file_name,
              const ExpectedColonInHeaderAssignmentInfo &info,
              const ErrorSeverity severity);

  void print_details() const override;

private:
  std::string file_name_;
  ParserErrorType type_;
  std::variant<MalformedSectionHeaderInfo, ExpectedColonInHeaderAssignmentInfo>
      info_;
};

void print_line_and_left_pad(std::optional<int> line_number);
