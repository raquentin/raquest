#include "errors/error_manager.hpp"
#include <fmt/color.h>
#include <fmt/core.h>

void ErrorManager::add_error(std::shared_ptr<Error> error) {
  std::lock_guard<std::mutex> lock(mutex_);
  errors_.emplace_back(std::move(error));
}

std::vector<std::shared_ptr<Error>> ErrorManager::get_errors() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return errors_;
}

void ErrorManager::print_all_errors() const {
  std::lock_guard<std::mutex> lock(mutex_);

  if (errors_.empty()) {
    return;
  }

  for (const auto &error : errors_) {
    error->print();
  }

  // Print error footer
  // TODO: fix the random % sign printed at the end of these.
  fmt::print(fg(fmt::terminal_color::bright_red) | fmt::emphasis::bold,
             "error");
  if (errors_.size() == 1) {
    fmt::print(fg(fmt::terminal_color::white),
               ": failed due to 1 errors above");
  } else {
    fmt::print(fg(fmt::terminal_color::white),
               ": failed due to {} errors above", errors_.size());
  }
}
