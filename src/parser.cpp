#include "parser.hpp"
#include "CLI11.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

Request Parser::parse_file(const std::string &file_path) {
  std::ifstream file(file_path);
  if (!file.is_open()) {
    throw std::runtime_error("failed to open file: " + file_path);
  }

  std::string line;
  std::string method, url;
  std::vector<std::string> headers;
  std::string json_payload;

  bool in_json_section = false;

  while (std::getline(file, line)) {
    line = CLI::detail::trim(line);
    if (line.empty() || line[0] == '#') {
      continue; // skip empty and comments
    }

    if (!in_json_section) {
      if (method.empty()) {
        std::istringstream request_line(line);
        request_line >> method >> url;
      } else if (line.find(":") != std::string::npos) {
        headers.push_back(line);
      } else {
        in_json_section = true;
        json_payload += line + "\n";
      }
    } else {
      json_payload += line + "\n";
    }
  }

  Request request(method, url);
  for (const auto &header : headers) {
    request.add_header(header);
  }

  if (!json_payload.empty()) {
    request.set_json_body(json_payload);
  }

  return request;
}
