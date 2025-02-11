#include <fmt/color.h>
#include <fmt/core.h>

#include "errors/curl_error.hpp"

CurlError::CurlError(const std::string &file_name, ErrorSeverity severity)
    : Error("curl error", file_name, severity) {}

void CurlError::print_details() const {
    fmt::print("\n");
    fmt::print("curl error");
    fmt::print("\n");
}
