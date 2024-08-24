#include <optional>
#include <string>

enum class ErrorType {
  SyntaxError,
  SemanticError,
  UnexpectedTokenError,
};

struct ParseError {
  ErrorType type;
  std::string message;
  int line_number;
  std::optional<std::string> section;
};
