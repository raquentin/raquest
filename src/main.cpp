#include "parser.hpp"
#include <CLI/CLI.hpp>
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  CLI::App app{"a command-line HTTP client"};

  app.add_flag("-v,--version", "print version and exit");
  std::string file_url;
  app.add_option("pos1,-f,--file", file_url, "specify a .raq file")->required()->check(CLI::ExistingFile);

  CLI11_PARSE(app, argc, argv);

  try {
    std::ifstream file(file_url);
    std::string file_as_str((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
    Parser parser = Parser(file_as_str);
    auto request = parser.parse();
    if (!request.has_value()) {
      for (auto error : parser.get_errors()) {
        std::cerr << error << std::endl;
      }
      return 1;
    }
    request->execute();
  // TODO: all errors as values, no try/catch
  } catch (const std::exception &ex) {
    std::cerr << "error: " << ex.what() << std::endl;
    return 1;
  }

  return 0;
}
