#pragma once

#include "fmt/color.h"
#include <optional>

// TODO: make this atomicish
inline void print_compiling(std::string file_name) {
    fmt::print(fg(fmt::terminal_color::bright_green) | fmt::emphasis::bold,
               "Compiling ");
    fmt::print(fg(fmt::terminal_color::white), "{}\n", file_name);
}

inline void print_running(std::string file_name) {
    fmt::print(fg(fmt::terminal_color::bright_cyan) | fmt::emphasis::bold,
               "Running   "); // 3 spaces to match width of Compiling
    fmt::print(fg(fmt::terminal_color::white), "{}\n", file_name);
}

inline std::string as_whitespace(int spaces) {
    return std::string(spaces, ' ');
}

// TODO: uninline this
inline void print_line_and_left_pad(std::optional<int> line_number) {
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

inline void print_final_error_footer(int errors_size) {
    fmt::print(fg(fmt::terminal_color::bright_red) | fmt::emphasis::bold,
               "error");
    if (errors_size == 1) {
        fmt::print(fg(fmt::terminal_color::white),
                   ": failed due to 1 errors above");
    } else {
        fmt::print(fg(fmt::terminal_color::white),
                   ": failed due to {} errors above", errors_size);
    }
}
