#pragma once

#include "error.hpp"
#include <fmt/color.h>
#include <fmt/core.h>
#include <string>

enum class RuntimeErrorType {
  Curl,
  Internal,
};

inline std::string runtime_error_type_as_error_title(RuntimeErrorType ret) {
  switch (ret) {
    case RuntimeErrorType::Curl:
      return "libcurl returned an error";
    case RuntimeErrorType::Internal:
      return "an internal error occured";
  }
}

struct RuntimeErrorInfo {
  std::string message;
};

class RuntimeError : public Error {
public:
  RuntimeError(const std::string &file_name, const std::string &message,
               const ErrorSeverity severity, const RuntimeErrorType type);

  void print_details() const override;

private:
  std::string file_name_;
  RuntimeErrorType type_;
  RuntimeErrorInfo info_;
};
