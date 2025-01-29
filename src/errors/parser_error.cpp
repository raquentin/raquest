#include "errors/parser_error.hpp"
#include <fmt/color.h>
#include <fmt/core.h>
#include "printer.hpp"
#include <variant>

ParserError::ParserError(const std::string &file_name,
                         const MalformedSectionHeaderInfo &info,
                         ErrorSeverity severity)
    : Error(ErrorKind::ParserError, "malformed section header", file_name,
            severity),
      type_(ParserErrorType::MalformedSectionHeader), info_(info) {}

ParserError::ParserError(const std::string &file_name,
                         const ExpectedColonInHeaderAssignmentInfo &info,
                         ErrorSeverity severity)
    : Error(ErrorKind::ParserError, "expected ':' in header assignment",
            file_name, severity),
      type_(ParserErrorType::ExpectedColonInHeaderAssignment), info_(info) {}

// print error-specific content.
// should all be indented
void ParserError::print_details() const {
  std::visit(
      [this](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, MalformedSectionHeaderInfo>) {
          std::string hint_text = "add closing bracket ^";
          print_line_and_left_pad(std::nullopt);
          fmt::print("\n");

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
        } else if constexpr (std::is_same_v<
                                 T, ExpectedColonInHeaderAssignmentInfo>) {
          print_line_and_left_pad(std::nullopt);
          fmt::print("\n");

          // print snippet
          print_line_and_left_pad(arg.line_number);
          fmt::print(fg(fmt::terminal_color::white), "{}{}\n",
                     as_whitespace(6 + arg.hint.message.size() + 1 -
                                   arg.hint.emph_range.first),
                     arg.snippet);

          // print hint
          print_line_and_left_pad(std::nullopt);
          fmt::print(fg(fmt::terminal_color::bright_cyan) | fmt::emphasis::bold,
                     "hint: ");
          fmt::print(fg(fmt::terminal_color::bright_cyan), "{}",
                     arg.hint.message);
          fmt::print(fg(fmt::terminal_color::bright_cyan) | fmt::emphasis::bold,
                     " {}\n",
                     std::string(arg.hint.emph_range.second -
                                     arg.hint.emph_range.first,
                                 '^'));

          // end with deadln
          print_line_and_left_pad(std::nullopt);
          fmt::print("\n");
        }
      },
      info_);
};
