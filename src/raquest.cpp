#include "raquest.hpp"
#include "assertion_set.hpp"
#include "parser.hpp"
#include <memory>

Raquest::Raquest(const std::string &file_name) : file_name_(file_name) {}

std::expected<CurlResponse, std::vector<std::unique_ptr<Error>>>
Raquest::run() const {
    Parser parser(file_name_);
    std::vector<std::unique_ptr<Error>> errors;

    // parser.parse()
    const auto parse_result = parser.parse();
    if (!parse_result) [[unlikely]] {
        const auto &parser_errors = parse_result.error();

        errors.reserve(parser_errors.size());
        for (auto &pe : parser_errors) {
            errors.push_back(std::make_unique<ParserError>(pe));
        }
        return std::unexpected(std::move(errors));
    }

    // curl_request.execute()
    const auto [curl_request, assertion_set] = parse_result.value();
    const auto curl_result = curl_request.execute();
    if (!curl_result) [[unlikely]] {
        errors.push_back(std::make_unique<CurlError>(curl_result.error()));
        return std::unexpected(std::move(errors));
    }

    // validate(assertions)
    const CurlResponse curl_response = curl_result.value();
    if (assertion_set.has_value()) {
        const auto failed_assertions =
            validate(curl_response, assertion_set.value());
        if (!failed_assertions.empty()) [[unlikely]] {
            errors.reserve(failed_assertions.size());
            for (auto &fa : failed_assertions) {
                errors.push_back(std::make_unique<AssertionError>(fa));
            }
            return std::unexpected(std::move(errors));
        }
    }

    return std::move(curl_response);
}
