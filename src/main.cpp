#include "errors/error_manager.hpp"
#include "errors/runtime_error.hpp"
#include "parser.hpp"
#include "version.hpp"
#include <CLI/CLI.hpp>
#include <future>
#include <memory>
#include <string>

int main(int argc, char **argv) {
  CLI::App app{"a command-line HTTP client"};

  app.set_version_flag("--version", std::string(RAQUEST_VERSION));

  std::vector<std::string> file_names;
  app.add_option("files", file_names, "specify one or more .raq files")
      ->required()
      ->check(CLI::ExistingFile);

  CLI11_PARSE(app, argc, argv);

  ErrorManager error_manager;

  std::vector<std::future<void>> futures;

  auto process_file = [&](const std::string &file_name) {
    try {
      Parser parser(file_name, error_manager);
      auto parse_result = parser.parse();

      if (!parse_result.has_value()) {
        return;
      }

      std::shared_ptr<Request> request = parse_result.value();
      request->execute();

      // TODO: Handle assertions and report any assertion errors similarly
    } catch (const std::exception &ex) {
      std::shared_ptr<Error> error = std::make_shared<RuntimeError>(
          file_name, ex.what(), ErrorSeverity::Error, RuntimeErrorType::Internal);
      error_manager.add_error(error);
    }
  };

  for (const auto &file_name : file_names) {
    futures.emplace_back(
        std::async(std::launch::async, process_file, file_name));
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
