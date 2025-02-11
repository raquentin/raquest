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
    FailedAssertion,
};

struct Hint {
    std::pair<int, int> emph_range;
    std::string message;
};

class Error {
  public:
    Error(const std::string &title, const std::string &file,
          ErrorSeverity severity)
        : title_(title), file_name_(file), severity_(severity) {}

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
        // <severity_>(kind_)
        switch (severity_) {
        case ErrorSeverity::Error:
            fmt::print(fg(fmt::terminal_color::red) | fmt::emphasis::bold,
                       "{} error", error_kind_to_string(kind_));
            break;

        case ErrorSeverity::Warning:
            fmt::print(fg(fmt::terminal_color::yellow) | fmt::emphasis::bold,
                       "{} warning", error_kind_to_string(kind_));
            break;
        case ErrorSeverity::FailedAssertion:
            fmt::print(fg(fmt::terminal_color::red) | fmt::emphasis::bold,
                       "assertion failed");
            break;
        }

        // : <title_>
        fmt::print(fmt::emphasis::bold, ": {}\n", title_);

        // *padding* -->
        // in cyan
        fmt::print(fg(fmt::terminal_color::bright_black) | fmt::emphasis::bold,
                   "   --> ");

        // <file_>
        fmt::print("{}\n", file_name_);

        // error-specific details
        print_details();

        // end with newline to separate errors
        fmt::print("\n");
    }

    ErrorKind get_code() const { return kind_; }
    std::string get_title() const { return title_; }
    std::string get_file() const { return file_name_; }
    ErrorSeverity get_severity() const { return severity_; }

  protected:
    ErrorKind kind_;
    std::string title_;
    std::string file_name_;
    ErrorSeverity severity_;
};
