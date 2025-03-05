#pragma once

#include "error.hpp"
#include <optional>
#include <string>
#include <variant>

struct MissingJsonField {
    int line_number;
    std::string snippet;
    std::optional<Hint> hint;
};

struct UnexpectedStatusCode {
    int line_number;
    std::string snippet;
    Hint hint;
};

class AssertionError final : public Error {
  public:
    template <typename T>
    AssertionError(const std::string &file_name, T &&info)
        : Error(file_name), info_{std::forward<T>(info)} {}

    void virtual print() const override;
    constexpr virtual std::string get_brief() const override;

  private:
    std::variant<MissingJsonField, UnexpectedStatusCode> info_;
};
