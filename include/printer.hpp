#pragma once

#include <error.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <mutex>
#include <optional>
#include <string>

#include "curl_response.hpp"
#include "errors/error.hpp"

#define SPACES(spaces) std::string((spaces), ' ')

/**
 * @brief A thread-safe logger for output and errors.
 */
class Printer {
  public:
    Printer();

    void compiling(const std::string &file_name) const;
    void running(const std::string &file_name) const;
    void retrying(const std::string &file_name) const;
    void error(const Error &error) const;
    void response(const CurlResponse &response) const;

    void print_line_and_left_pad(std::optional<int> line_number) const;

    /**
     * @brief Print information summarizing the program's output. To be called
     * after all futures resolve.
     */
    void error_footer(int errors_size) const;

    friend int main(int argc, char **argv);

  private:
    mutable std::mutex mutex_;

    bool verbose_;
    bool quiet_;
    bool color_;
};

/**
 * @brief Provides a global static printer object.
 */
Printer &printer();
