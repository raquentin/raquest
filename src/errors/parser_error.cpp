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
    fmt::print(fg(fmt::terminal_color::bright_black) | fmt::emphasis::bold,
               " {}{}| ", line_number.value(), as_whitespace(3 - digits));
  } else {
    fmt::print(fg(fmt::terminal_color::bright_black) | fmt::emphasis::bold,
               "    | ");
  }
}

// print error-specific content.
// should all be indented
void ParserError::print_details() const {
  std::visit(
      [this](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        // MalformedSectionHeaderInfo
        if constexpr (std::is_same_v<T, MalformedSectionHeaderInfo>) {
          // lead with deadln
          print_line_and_left_pad(std::nullopt);
          fmt::print("\n");

          std::string hint_text = "add closing bracket ^";

          // print snippet
          print_line_and_left_pad(arg.line_number);
          fmt::print(
              fg(fmt::terminal_color::white), "{}{}\n",
              as_whitespace(6 + hint_text.length() - arg.snippet.length() - 1),
              arg.snippet);

          // print hint
          print_line_and_left_pad(std::nullopt);
          fmt::print(fg(fmt::terminal_color::bright_cyan) | fmt::emphasis::bold,
                     "hint: ");
          fmt::print(fg(fmt::terminal_color::bright_cyan),
                     "add closing bracket ^\n");

          // end with deadln
          print_line_and_left_pad(std::nullopt);
          fmt::print("\n");
        }
      },
      info_);
};
