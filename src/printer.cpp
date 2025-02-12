#include "printer.hpp"

Printer &printer() {
    static Printer instance;
    return instance;
}

void Printer::compiling(const std::string &file_name) const {
    fmt::print(fg(fmt::terminal_color::bright_green) | fmt::emphasis::bold,
               "Compiling ");
    fmt::print(fg(fmt::terminal_color::white), "{}\n", file_name);
}

void Printer::running(const std::string &file_name) const {
    fmt::print(fg(fmt::terminal_color::bright_cyan) | fmt::emphasis::bold,
               "Running   ");
    fmt::print(fg(fmt::terminal_color::white), "{}\n", file_name);
}

void Printer::error(Error &error) const {
    // Error: <title_>
    fmt::print(fg(fmt::terminal_color::red) | fmt::emphasis::bold, "Error");

    fmt::print(fmt::emphasis::bold, ": {}\n", error.get_brief());

    // --> <file_name_>
    fmt::print(fg(fmt::terminal_color::bright_black) | fmt::emphasis::bold,
               "   --> ");
    fmt::print("{}\n", error.file_name_);

    // error-specific details
    error.print();

    // end with newline to separate errors
    fmt::print("\n");
}

void Printer::print_line_and_left_pad(std::optional<int> line_number) const {
    int digits = 0;
    if (line_number.has_value()) {
        while (line_number.value() != 0) {
            line_number.value() /= 10;
            digits++;
        }
        fmt::print(fg(fmt::terminal_color::bright_black) | fmt::emphasis::bold,
                   " {}{}| ", line_number.value(), SPACES(3 - digits));
    } else {
        fmt::print(fg(fmt::terminal_color::bright_black) | fmt::emphasis::bold,
                   "    | ");
    }
}

void Printer::error_footer(int errors_size) const {
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
