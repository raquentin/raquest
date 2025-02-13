#pragma once

#include "error.hpp"
#include <string>

class CurlError final : public Error {
  public:
    CurlError(const std::string &file_name, const std::string &curl_error);

    void virtual print() const override;
    constexpr virtual std::string get_brief() const override;

  private:
    const std::string curl_error_;
};
