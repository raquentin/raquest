#include "CLI11.hpp"
#include "parser.hpp"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv) {
  // CLI11 setup
  CLI::App app{"a command-line HTTP client"};

  app.add_flag("-v,--version", "print version and exit");
  string file_url;
  app.add_option("-f,--file", file_url, "specify a .raq file")->required();

  CLI11_PARSE(app, argc, argv);

  try {
    Request request = Parser::parse_file(file_url);
    request.execute();
  } catch (const std::exception &ex) {
    std::cerr << "error: " << ex.what() << std::endl;
    return 1;
  }

  return 0;
}
