#pragma once
#include "error.hpp"
#include <optional>
#include <string>
#include <variant>

struct Hint {
    std::pair<int, int> emph_range;
    std::string message;
};

struct MalformedSectionHeader {
    std::string section_name;
    int line_number;
    std::string snippet;
    std::optional<Hint> hint;
};

struct ExpectedColonInHeaderAssignment {
    int line_number;
    std::string snippet;
    Hint hint;
};

class ParserError final : public Error {
  public:
    template <typename T>
    ParserError(const std::string &file_name, T &&info)
        : Error(file_name), info_{std::forward<T>(info)} {}

    void virtual print() const override;
    constexpr virtual std::string get_brief() const override;

  private:
    std::variant<MalformedSectionHeader, ExpectedColonInHeaderAssignment> info_;
};
