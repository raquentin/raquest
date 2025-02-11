#pragma once
#include <map>
#include <optional>
#include <string>

class CurlResponse {
  public:
    int get_status_code() const;
    std::optional<std::string> get_header_value(const std::string &key) const;
    std::optional<std::string> get_json_field(const std::string &key) const;
    std::string get_body() const;
    std::string get_file_name() const { return file_name_; }

    void parse_status_code(long code);
    void parse_header(const std::string &header_line);
    void set_body(const std::string &data);
    void set_file_name(const std::string &file_name) { file_name_ = file_name; }

  private:
    std::string file_name_;
    int status_code;
    std::map<std::string, std::string> headers;
    std::string body;
};
