#include "assert.hpp"
#include "errors/error_manager.hpp"
#include "errors/runtime_error.hpp"
#include "parser.hpp"
#include "version.hpp"
#include <CLI/CLI.hpp>
#include <filesystem>
#include <future>
#include <memory>
#include <string>

std::vector<std::string>
collect_raq_files(const std::vector<std::string> &paths, bool recursive,
                  ErrorManager &error_manager) {
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
            if (entry.is_regular_file() && entry.path().extension() == ".raq") {
              raq_files.emplace_back(entry.path().string());
            }
          }
        } else {
          for (const auto &entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file() && entry.path().extension() == ".raq") {
              raq_files.emplace_back(entry.path().string());
            }
          }
        }
      } else {
        std::shared_ptr<Error> error = std::make_shared<RuntimeError>(
            path_str, "path is neither a regular file nor a directory.",
            ErrorSeverity::Warning, RuntimeErrorType::FileSystem);
        error_manager.add_error(error);
      }
    } catch (const std::filesystem::filesystem_error &e) {
      std::shared_ptr<Error> error = std::make_shared<RuntimeError>(
          path_str, e.what(), ErrorSeverity::Warning,
          RuntimeErrorType::FileSystem);
      error_manager.add_error(error);
    }
  }
  return raq_files;
}

int main(int argc, char **argv) {
  CLI::App app{"a command-line HTTP client"};

  app.set_version_flag("--version", std::string(RAQUEST_VERSION));

  bool recursive = false;
  app.add_flag("-r,--recursive", recursive,
               "recursively search directories for .raq files");

  std::vector<std::string> paths;
  app.add_option("paths", paths,
                 "specify one or more .raq files or directories")
      ->required()
      ->check(CLI::ExistingPath)
      ->expected(-1, 1);

  CLI11_PARSE(app, argc, argv);

  ErrorManager error_manager;

  std::vector<std::string> raq_files =
      collect_raq_files(paths, recursive, error_manager);

  if (raq_files.empty()) {
    std::shared_ptr<Error> error = std::make_shared<RuntimeError>(
        "<no file>", "no .raq files found in the specified paths.",
        ErrorSeverity::Error, RuntimeErrorType::Internal);
    error_manager.add_error(error);
  }

  unsigned int max_concurrent_threads = std::thread::hardware_concurrency();
  if (max_concurrent_threads == 0) {
    max_concurrent_threads = 4;
  }

  std::counting_semaphore<> semaphore(max_concurrent_threads);

  std::vector<std::future<void>> futures;

  auto process_file = [&](const std::string &file_name) {
    semaphore.acquire();
    try {
      Parser parser(file_name, error_manager);
      auto parse_result = parser.parse();

      if (!parse_result.has_value()) {
        semaphore.release();
        return;
      }

      std::shared_ptr<Request> request = parse_result.value();
      auto execute_result = request->execute();

      if (!execute_result.has_value()) {
        semaphore.release();
        return;
      }

      // TODO: print execute_result->data if successful and right mode.

      // validate assertions
      validate(execute_result.value(), request->get_assertion_set(),
               error_manager);
    } catch (const std::exception &ex) {
      std::shared_ptr<Error> error = std::make_shared<RuntimeError>(
          file_name, ex.what(), ErrorSeverity::Error,
          RuntimeErrorType::Internal);
      error_manager.add_error(error);
    }
    semaphore.release();
  };

  for (const auto &raq_file : raq_files) {
    futures.emplace_back(
        std::async(std::launch::async, process_file, raq_file));
  }

  for (auto &fut : futures) {
    fut.get();
  }

  if (!error_manager.get_errors().empty()) {
    error_manager.print_all_errors();
    return 1;
  }

  return 0;
}
