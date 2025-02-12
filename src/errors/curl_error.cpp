#include "errors/curl_error.hpp"
#include <fmt/format.h>

CurlError::CurlError(const std::string &file_name, Type type)
    : Error(file_name), type_(type) {}

void CurlError::print() const { fmt::print("CurlError::print()\n"); }

constexpr inline std::string CurlError::get_brief() const {
    switch (type_) {
        using enum Type;
    case UnitializedCurl:
        return "libcurl not initiazlied";
    default:
        return "unknown curl error";
    }
};
