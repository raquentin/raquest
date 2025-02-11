#include <fmt/color.h>
#include <fmt/core.h>

#include "errors/file_error.hpp"

FileError::FileError(const std::string &file_name, ErrorSeverity severity)
    : Error("file error", file_name, severity) {}

void FileError::print_details() const {
    fmt::print("\n");
    fmt::print("file error");
    fmt::print("\n");
}
