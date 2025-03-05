#include "printer.hpp"
#include "config.hpp"
#include "curl_response.hpp"

Printer::Printer()
    : verbose_(config().verbose), quiet_(config().quiet),
      color_(!config().no_color) {}

void Printer::compiling(const std::string &file_name) const {
    if (!verbose_)
        return;

    std::unique_lock<std::mutex> lock(mutex_);

    fmt::print(fg(fmt::terminal_color::bright_green) | fmt::emphasis::bold,
               "Compiling ");
    fmt::print(fg(fmt::terminal_color::white), "{}\n", file_name);
}

void Printer::running(const std::string &file_name) const {
    if (!verbose_)
        return;

    std::unique_lock<std::mutex> lock(mutex_);

    fmt::print(fg(fmt::terminal_color::bright_cyan) | fmt::emphasis::bold,
               "Running   ");
    fmt::print(fg(fmt::terminal_color::white), "{}\n", file_name);
}

void Printer::retrying(const std::string &file_name) const {
    if (!verbose_)
        return;

    std::unique_lock<std::mutex> lock(mutex_);

    fmt::print(fg(fmt::terminal_color::yellow) | fmt::emphasis::bold,
               "Retrying  ");
    fmt::print(fg(fmt::terminal_color::white), "{}\n", file_name);
}

void Printer::error(const Error &error) const {
    std::unique_lock<std::mutex> lock(mutex_);

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

void Printer::response(const CurlResponse &response) const {
    std::unique_lock<std::mutex> lock(mutex_);

    fmt::print("{}\n", response.get_body());
}

/**
 * @note Should not acquire the mutex because this will called from functions
 * with the mutex.
 */
void Printer::print_line_and_left_pad(std::optional<int> line_number) const {
    int digits = 0;
    if (line_number.has_value()) {
        auto val = line_number.value();
        while (val != 0) {
            val /= 10;
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
    std::unique_lock<std::mutex> lock(mutex_);

    fmt::print(fg(fmt::terminal_color::bright_red) | fmt::emphasis::bold,
               "error");
    if (errors_size == 1) {
        fmt::print(fg(fmt::terminal_color::white),
                   ": failed due to 1 error above");
    } else {
        fmt::print(fg(fmt::terminal_color::white),
                   ": failed due to {} errors above", errors_size);
    }
}

Printer &printer() {
    static Printer instance;
    return instance;
}
