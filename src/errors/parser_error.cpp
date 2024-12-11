#include "errors/parser_error.hpp"
#include <fmt/color.h>
#include <fmt/core.h>
#include <variant>

ParserError::ParserError(const std::string &file_name,
                         const MalformedSectionHeaderInfo &info,
                         ErrorSeverity severity)
    : Error(ErrorKind::ParserError, "malformed section header", file_name,
            severity),
      type_(ParserErrorType::MalformedSectionHeader), info_(info) {}

void print_line_and_left_pad(std::optional<int> line_number) {
  int digits = 0;
  if (line_number.has_value()) {
    while (line_number.value() != 0) {
      line_number.value() /= 10;
      digits++;
    }
    fmt::print(fg(fmt::terminal_color::bright_black) | fmt::emphasis::bold, " {}{}| ", line_number.value(),
               std::string(3 - digits, ' '));
  } else {
    fmt::print(fg(fmt::terminal_color::bright_black) | fmt::emphasis::bold, "    | ");
  }
}

// print error-specific content.
// should all be indented
void ParserError::print_details() const {
  std::visit(
      [this](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, MalformedSectionHeaderInfo>) {
          // lead with deadln
          print_line_and_left_pad(std::nullopt);
          fmt::print("\n");

          print_line_and_left_pad(arg.line_number);
          fmt::print(fg(fmt::terminal_color::white), "{}\n", arg.snippet);

          // end with deadln
          print_line_and_left_pad(std::nullopt);
          fmt::print("\n");
        }
      },
      info_);
};
