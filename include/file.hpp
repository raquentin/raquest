#include "errors/file_error.hpp"
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

std::expected<std::vector<std::string>, FileError> inline collect_raq_files(
    const std::vector<std::string> &paths, bool recursive) {
    std::vector<std::string> raq_files;
    for (const auto &path_str : paths) {
        std::filesystem::path path(path_str);
        try {
            if (std::filesystem::is_regular_file(path)) {
                if (path.extension() == ".raq") {
                    raq_files.emplace_back(path.string());
                }
            } else if (std::filesystem::is_directory(path)) {
                if (recursive) {
                    for (const auto &entry :
                         std::filesystem::recursive_directory_iterator(path)) {
                        if (entry.is_regular_file() &&
                            entry.path().extension() == ".raq") {
                            raq_files.emplace_back(entry.path().string());
                        }
                    }
                } else {
                    for (const auto &entry :
                         std::filesystem::directory_iterator(path)) {
                        if (entry.is_regular_file() &&
                            entry.path().extension() == ".raq") {
                            raq_files.emplace_back(entry.path().string());
                        }
                    }
                }
            } else {
                return std::unexpected(
                    FileError(path_str, FileError::Type::BadPath));
            }
        } catch (const std::filesystem::filesystem_error &e) {
            return std::unexpected(
                FileError(path_str, FileError::Type::Filesystem));
        }
    }
    return raq_files;
}
