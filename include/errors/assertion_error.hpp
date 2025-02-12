#pragma once

#include "error.hpp"
#include <string>

class AssertionError final : public Error {
  public:
    enum class Type {
        UnexpectedStatusCode,
        MissingHeader,
        UnexpectedHeader,
        MissingJsonField,
        UnexpectedJsonField,
    };

    AssertionError(const std::string &file_name, const Type type);

    void virtual print() const override;

    constexpr virtual std::string get_brief() const override;

  private:
    using enum Type;
    Type type_;
};
