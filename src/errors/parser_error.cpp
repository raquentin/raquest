#include "errors/parser_error.hpp"
#include "printer.hpp"
#include <fmt/color.h>
#include <fmt/core.h>
#include <variant>

// print error-specific content.
// should all be indented
void ParserError::print() const {
    std::visit(
        [](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, MalformedSectionHeader>) {
                std::string hint_text = "add closing bracket ^";
                printer().print_line_and_left_pad(std::nullopt);
                fmt::print("\n");

                // print snippet
                printer().print_line_and_left_pad(arg.line_number);
                fmt::print(
                    fg(fmt::terminal_color::white), "{}{}\n",
                    SPACES(6 + hint_text.length() - arg.snippet.length() - 1),
                    arg.snippet);

                // print hint
                printer().print_line_and_left_pad(std::nullopt);
                fmt::print(fg(fmt::terminal_color::bright_cyan) |
                               fmt::emphasis::bold,
                           "hint: ");
                fmt::print(fg(fmt::terminal_color::bright_cyan),
                           "add closing bracket ^\n");

                // end with deadln
                printer().print_line_and_left_pad(std::nullopt);
                fmt::print("\n");
            } else if constexpr (std::is_same_v<
                                     T, ExpectedColonInHeaderAssignment>) {
                printer().print_line_and_left_pad(std::nullopt);
                fmt::print("\n");

                // print snippet
                printer().print_line_and_left_pad(arg.line_number);
                fmt::print(fg(fmt::terminal_color::white), "{}{}\n",
                           SPACES(6 + arg.hint.message.size() + 1 -
                                  arg.hint.emph_range.first),
                           arg.snippet);

                // print hint
                printer().print_line_and_left_pad(std::nullopt);
                fmt::print(fg(fmt::terminal_color::bright_cyan) |
                               fmt::emphasis::bold,
                           "hint: ");
                fmt::print(fg(fmt::terminal_color::bright_cyan), "{}",
                           arg.hint.message);
                fmt::print(fg(fmt::terminal_color::bright_cyan) |
                               fmt::emphasis::bold,
                           " {}\n",
                           std::string(arg.hint.emph_range.second -
                                           arg.hint.emph_range.first,
                                       '^'));

                // end with deadln
                printer().print_line_and_left_pad(std::nullopt);
                fmt::print("\n");
            }
        },
        info_);
};

constexpr inline std::string ParserError::get_brief() const {
    return std::visit(
        [](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, MalformedSectionHeader>)
                return "malformed section header";
            else if constexpr (std::is_same_v<T,
                                              ExpectedColonInHeaderAssignment>)
                return "missing colon in header assignment";
        },
        info_);
}
