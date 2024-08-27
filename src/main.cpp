#include "cli.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "request.hpp"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv) {
  CLI::App app{"a command-line HTTP client"};

  app.add_flag("-v,--version", "print version and exit");
  string file_url;
  app.add_option("-f,--file", file_url, "specify a .raq file")->required();

  CLI11_PARSE(app, argc, argv);

  try {
    Lexer lexer = Lexer{file_url};
    auto tokens = lexer.tokenize();
    if (!tokens.has_value()) {
      for (auto error : lexer.errors) {
        std::cerr << error.message << std::endl;
      }
    }
    Parser parser;
    auto request = parser.parse(tokens.value());
    if (!request.has_value()) {
      for (auto error : parser.errors) {
        std::cerr << error.message << std::endl;
      }
    }
    request->execute();
  } catch (const std::exception &ex) {
    std::cerr << "error: " << ex.what() << std::endl;
    return 1;
  }

  return 0;
}
