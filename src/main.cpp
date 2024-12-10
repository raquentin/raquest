#include "parser.hpp"
#include "version.hpp"
#include <CLI/CLI.hpp>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  CLI::App app{"a command-line HTTP client"};

  app.set_version_flag("--version", std::string(RAQUEST_VERSION));

  std::string file_url;
  app.add_option("pos1,-f,--file", file_url, "specify a .raq file")
      ->required()
      ->check(CLI::ExistingFile);

  CLI11_PARSE(app, argc, argv);

  try {
    Parser parser = Parser(file_url);
    auto request = parser.parse();
    if (!parser.errors.empty()) {
      // TODO: print error head
      for (ParserError error : parser.errors) {
        error.print();
      }
      // TODO: print error foot
    } else {
      request->execute();
    }
  } catch (const std::exception &ex) {
    std::cerr << "error: " << ex.what() << std::endl;
    return 1;
  }

  return 0;
}
