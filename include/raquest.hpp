#pragma once

#include "app_context.hpp"
#include "errors/error.hpp"
#include <expected>
#include <string>
#include <vector>

class Raquest {
  public:
    Raquest(const std::string &file_name);

    /**
     * @brief Executes a .raq file.
     *
     * This function parses the file, calls libcurl to make
     * the web request, and validates assertions if they
     * exist.
     *
     * @return A vector of Errors (Parser, Curl, or Assertion).
     */
    std::vector<std::unique_ptr<Error>> run() const;

    /**
     * @brief Prints all output thread-safely to std::cout;
     */
    void print_result(AppContext &app_config);

  private:
    const std::string file_name_;
};
