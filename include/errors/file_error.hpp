#pragma once

#include "error.hpp"
#include <string>

enum class FileErrorType {
    The,
};

class FileError : public Error {
  public:
    FileError(const std::string &file_name, ErrorSeverity severity);

    void print_details() const override;

  private:
    FileErrorType type_;
};
