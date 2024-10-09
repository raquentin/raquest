#include <string>

enum class ErrorType {
  UnexpectedCharacter,
  UnterminatedStringLiteral,
  MalinformedSectionHeader,
  ExpectedIdentifier,
  UnexpectedIdentifier,
  UnexpectedEndOfFile,
};

class Error {
public:
  Error(ErrorType type, const std::string &line_content, int line_number, int column_number);
  std::string pretty_sprint() const;
  friend std::ostream &operator<<(std::ostream &os, const Error &error);

private:
  ErrorType type;
  std::string line_content;
  int line_number;
  int column_number;
};
