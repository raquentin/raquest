#include "errors/runtime_error.hpp"

RuntimeError::RuntimeError(const std::string &file_name,
                           const std::string &message,
                           const ErrorSeverity severity,
                           const RuntimeErrorType type)
    : Error(ErrorKind::RuntimeError, runtime_error_type_as_error_title(type), file_name, severity), type_(type),
      info_({message}) {}

void RuntimeError::print_details() const {
  fmt::print(fg(fmt::terminal_color::white), "{}\n", info_.message);
}
