#pragma once

#include "fmt/color.h"

// TODO: make this atomicish
inline void print_compiling(std::string file_name) {
  fmt::print(fg(fmt::terminal_color::bright_green) | fmt::emphasis::bold, "Compiling ");
  fmt::print(fg(fmt::terminal_color::white), "{}\n", file_name);
}

inline void print_running(std::string file_name) {
  fmt::print(fg(fmt::terminal_color::bright_cyan) | fmt::emphasis::bold, "Running ");
  fmt::print(fg(fmt::terminal_color::white), "{}\n", file_name);
}
