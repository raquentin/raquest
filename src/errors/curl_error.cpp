#include "errors/curl_error.hpp"
#include <fmt/format.h>

CurlError::CurlError(const std::string &file_name,
                     const std::string &curl_error)
    : Error(file_name), curl_error_(curl_error) {}

void CurlError::print() const { fmt::print("CurlError::print()\n"); }

constexpr inline std::string CurlError::get_brief() const {
    return curl_error_;
};
