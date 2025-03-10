#include "curl_response.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int CurlResponse::get_status_code() const { return status_code; }

std::optional<std::string>
CurlResponse::get_header_value(const std::string &key) const {
    auto it = headers.find(key);
    if (it != headers.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<std::string>
CurlResponse::get_json_field(const std::string &key) const {
    try {
        auto json = json::parse(body);

        if (json.contains(key)) {
            return json[key].dump();
        } else {
            return std::nullopt;
        }
    } catch (json::parse_error &e) {
        // TODO: custom error here
        return std::nullopt;
    }
}

std::string CurlResponse::get_body() const { return body; }

void CurlResponse::parse_status_code(long code) {
    status_code = static_cast<int>(code);
}

void CurlResponse::parse_header(const std::string &header_line) {
    size_t separator_pos = header_line.find(':');

    if (separator_pos != std::string::npos) {
        std::string key = header_line.substr(0, separator_pos);
        std::string value = header_line.substr(separator_pos + 1);

        key.erase(key.find_last_not_of(" \n\r\t") + 1);
        value.erase(0, value.find_first_not_of(" \n\r\t"));

        headers[key] = value;
    }
}

void CurlResponse::set_body(const std::string &data) { body = data; }
