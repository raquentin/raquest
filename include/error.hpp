#include <string>

enum class ErrorType {
  UnexpectedCharacter,
  UnterminatedStringLiteral,
  MalinformedSectionHeader,
};

struct Error {
  ErrorType type;
  std::string message;
  int line_number;
  int column_number;
};
