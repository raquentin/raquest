#pragma once

#include <string>

/**
 * @brief A virtual Error class.
 */
class Error {
  public:
    explicit Error(const std::string &file_name) : file_name_(file_name) {}

    virtual ~Error() = default;

    /**
     * @brief Print Error derivation-specific details. General parts, like
     * <file_name_> and brief, are printed directly in printer::error().
     */
    virtual void print() const = 0;

    /**
     * @brief A constexpr to map inner error Type enumerators to
     * short titles for error output headings.
     */
    virtual std::string get_brief() const = 0;

    const std::string file_name_;
};
