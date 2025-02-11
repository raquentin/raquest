#pragma once

#include "error.hpp"
#include <string>

enum class CurlErrorType {
    The,
};

class CurlError : public Error {
  public:
    CurlError(const std::string &file_name, ErrorSeverity severity);

    void print_details() const override;

  private:
    CurlErrorType type_;
};
