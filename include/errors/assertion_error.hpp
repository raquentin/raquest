#pragma once

#include "error.hpp"
#include <string>

enum class AssertionErrorType {
    UnexpectedStatusCode,
    MissingHeader,
    UnexpectedHeader,
    MissingJsonField,
    UnexpectedJsonField,
};

constexpr inline std::string
assertion_error_type_as_error_title(AssertionErrorType aet) {
    switch (aet) {
    case AssertionErrorType::UnexpectedStatusCode:
        return "unexpected status code";
    case AssertionErrorType::MissingHeader:
        return "missing header";
    case AssertionErrorType::UnexpectedHeader:
        return "unexpected header";
    case AssertionErrorType::MissingJsonField:
        return "missing json field";
    case AssertionErrorType::UnexpectedJsonField:
        return "unexpected json field";
    }
}

class AssertionError : public Error {
  public:
    AssertionError(const std::string &file_name, const std::string snippet,
                   AssertionErrorType type);

    void print_details() const override;

  private:
    std::string snippet_;
};
