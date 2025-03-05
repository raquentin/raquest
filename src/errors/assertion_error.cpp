#include "errors/assertion_error.hpp"
#include "printer.hpp"
#include <fmt/core.h>
#include <fmt/format.h>
#include <optional>
#include <variant>

void AssertionError::print() const {
    std::visit(
        [](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, MissingJsonField>) {
                // skip first line
                printer().print_line_and_left_pad(std::nullopt);
                fmt::print("\n");

                // print snippet
                printer().print_line_and_left_pad(arg.line_number);
                fmt::print(fg(fmt::terminal_color::white), "{}\n", arg.snippet);

                // print emph
                printer().print_line_and_left_pad(std::nullopt);
                fmt::print(fg(fmt::terminal_color::white), "{}{}\n",
                           SPACES(arg.hint->emph_range.first),
                           std::string(arg.hint->emph_range.second -
                                           arg.hint->emph_range.first,
                                       '^'));

                // print hint
                printer().print_line_and_left_pad(std::nullopt);
                fmt::print(fg(fmt::terminal_color::bright_cyan), "expl: {}\n",
                           arg.hint->message);
            } else if constexpr (std::is_same_v<T, UnexpectedStatusCode>) {
                printer().print_line_and_left_pad(std::nullopt);
            };
        },
        info_);
}

constexpr inline std::string AssertionError::get_brief() const {
    return std::visit(
        [](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, MissingJsonField>)
                return "missing json field";
            else if constexpr (std::is_same_v<T, UnexpectedStatusCode>)
                return "missing colon in header assignment";
        },
        info_);
}
