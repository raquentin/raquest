#pragma once
#include "error.hpp"
#include <optional>
#include <string>
#include <variant>

enum class ParserErrorType {
  MalformedSectionHeader,
};

struct Hint {
  std::pair<int, int> emph_range;
  std::string message;
};

struct MalformedSectionHeaderInfo {
  std::string section_name;
  int line_number;
  std::string snippet;
  std::optional<Hint> hint;
};

class ParserError : public Error {
public:
  ParserError(const std::string &file_name,
              const MalformedSectionHeaderInfo &info,
              const ErrorSeverity severity);

  void print_details() const override;

private:
  std::string file_name_;
  ParserErrorType type_;
  std::variant<MalformedSectionHeaderInfo> info_;
};

void print_line_and_left_pad(std::optional<int> line_number);
