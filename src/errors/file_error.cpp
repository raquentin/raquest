#include <fmt/color.h>
#include <fmt/core.h>

#include "errors/file_error.hpp"

FileError::FileError(const std::string &file_name, Type type)
    : Error(file_name), type_(type) {}

void FileError::print() const { fmt::print("FileError::print()\n"); }

constexpr inline std::string FileError::get_brief() const {
    switch (type_) {
        using enum Type;
    case Filesystem:
        return "filesystem error";
    case BadPath:
        return "invalid file/directory path";
    default:
        return "unknown file error";
    }
};
