#pragma once
#include <fmt/color.h>
#include <fmt/core.h>
#include <string>

enum class ErrorKind {
  ParserError,
  RuntimeError,
  AssertionError,
};

enum class ErrorSeverity {
  Error,
  Warning,
};

class Error {
public:
  Error(ErrorKind code, const std::string &title, const std::string &file,
        ErrorSeverity severity)
      : kind_(code), title_(title), file_(file), severity_(severity) {}

  virtual ~Error() = default;

  virtual void print_details() const = 0;

  // for use in printing [`ErrorKind`] in errors
  std::string error_kind_to_string(ErrorKind kind) const {
    switch (kind) {
    case ErrorKind::ParserError:
      return "parser";
    case ErrorKind::AssertionError:
      return "assertion";
    case ErrorKind::RuntimeError:
      return "runtime";
    }
  }

  void print() const {

    std::string severity_str;
    fmt::terminal_color color;

    if (severity_ == ErrorSeverity::Error) {
      severity_str = "error";
      color = fmt::terminal_color::red;
    } else if (severity_ == ErrorSeverity::Warning) {
      severity_str = "warning";
      color = fmt::terminal_color::yellow;
    }

    // <severity_>(kind_)
    // color based on severity above
    fmt::print(fg(color) | fmt::emphasis::bold, "{}({})", severity_str,
               error_kind_to_string(kind_));

    // : <title_>
    fmt::print(fmt::emphasis::bold, ": {}\n", title_);

    // *padding* -->
    // in cyan
    fmt::print(fg(fmt::terminal_color::bright_black) | fmt::emphasis::bold,
               "   --> ");

    // <file_>
    fmt::print("{}\n", file_);

    // error-specific details
    print_details();

    // end with newline to separate errors
    fmt::print("\n");
  }

  ErrorKind get_code() const { return kind_; }
  std::string get_title() const { return title_; }
  std::string get_file() const { return file_; }
  ErrorSeverity get_severity() const { return severity_; }

protected:
  ErrorKind kind_;
  std::string title_;
  std::string file_;
  ErrorSeverity severity_;
};

inline std::string as_whitespace(int spaces) {
  return std::string(spaces, ' ');
}
