#include "errors/assertion_error.hpp"
#include <fmt/format.h>

AssertionError::AssertionError(const std::string &file_name, Type type)
    : Error(file_name), type_(type) {}

void AssertionError::print() const { fmt::print("AssertionError::print()"); }

constexpr inline std::string AssertionError::get_brief() const {
    switch (type_) {
        using enum Type;
    case UnexpectedStatusCode:
        return "unexpected status code";
    case MissingHeader:
        return "missing header";
    case UnexpectedHeader:
        return "unexpected header";
    case MissingJsonField:
        return "missing json field";
    case UnexpectedJsonField:
        return "unexpected json field";
    default:
        return "unknown assertion error";
    }
}
