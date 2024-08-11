#include "parser.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

Request Parser::parse_file(const std::string &file_path) {
  std::ifstream file(file_path);
  if (!file.is_open()) {
    throw std::runtime_error("failed to open file: " + file_path);
  }

  std::string line;
  std::getline(file, line);
  std::istringstream request_line(line);

  std::string method, url;
  request_line >> method >> url;

  Request request(method, url);

  while (std::getline(file, line)) {
    if (!line.empty() && line[0] == '#') {
      request.add_header(line.substr(1));
    }
  }

  return request;
}
