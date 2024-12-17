#include "errors/assertion_error.hpp"

// TODO: get errorkind as message fn
AssertionError::AssertionError(const std::string &file_name,
                               const std::string &message,
                               AssertionErrorType type)
    : Error(ErrorKind::AssertionError, message, file_name,
            ErrorSeverity::FailedAssertion),
      type_(type), message_(message) {}

void AssertionError::print_details() const {}
