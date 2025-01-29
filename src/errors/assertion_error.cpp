#include "errors/assertion_error.hpp"
#include "printer.hpp"

// TODO: get errorkind as message fn
AssertionError::AssertionError(const std::string &file_name,
                               const std::string snippet,
                               AssertionErrorType type)
    : Error(ErrorKind::AssertionError,
            assertion_error_type_as_error_title(type), file_name,
            ErrorSeverity::FailedAssertion),
      snippet_(snippet) {}

void AssertionError::print_details() const {

  // print snippet
  fmt::print(fg(fmt::terminal_color::white), snippet_);

  // end with deadln
  print_line_and_left_pad(std::nullopt);
  fmt::print("\n");
}
