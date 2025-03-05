#include <CLI/CLI.hpp>
#include <curl/curl.h>
#include <future>
#include <iostream>
#include <memory>
#include <string>

#include "config.hpp"
#include "file.hpp"
#include "printer.hpp"
#include "raquest.hpp"

import ThreadPool;

#define RAQUEST_VERSION "0.0.1"

int main(int argc, char **argv) {
    curl_global_init(CURL_GLOBAL_ALL);

    CLI::App app{"a command-line HTTP client"};

    app.set_version_flag("--version", RAQUEST_VERSION);

    app.add_option("-j, --jobs", config().jobs,
                   "an upper bound for concurrency")
        ->capture_default_str();
    ;

    auto cli_output_modifiers = app.add_option_group("output_modifiers");
    cli_output_modifiers->add_flag(
        "-v, --verbose", config().verbose,
        "print potentially unnessesary console output");
    cli_output_modifiers->add_flag("-q, --quiet", config().quiet,
                                   "only print critical error output");

    app.add_flag("--no_color", config().no_color,
                 "disable output color and text decoration");

    app.add_option("paths", config().input_paths,
                   "specify one or more .raq files or directories")
        ->required()
        ->check(CLI::ExistingPath);

    app.add_flag("-r,--recursive", config().recursive,
                 "recursively search directories for .raq files")
        ->capture_default_str();

    app.add_option("-R, --retries", config().retries,
                   "specify one or more .raq files or directories")
        ->capture_default_str();

    CLI11_PARSE(app, argc, argv);

    auto input_filenames =
        collect_raq_files(config().input_paths, config().recursive);

    if (!input_filenames.has_value()) [[unlikely]] {
        std::cerr << "no input filenames error\n";
        std::exit(EXIT_FAILURE);
    }

    std::vector<std::future<
        std::expected<CurlResponse, std::vector<std::unique_ptr<Error>>>>>
        futures;

    ThreadPool pool(config().jobs);
    for (const auto &filename : input_filenames.value()) {
        futures.emplace_back(
            pool.submit([filename] { return Raquest(filename).run(); }));
    }

    for (auto &fut : futures) {
        std::expected<CurlResponse, std::vector<std::unique_ptr<Error>>>
            raquest_result = fut.get();

        if (raquest_result.has_value()) {
            printer().response(*raquest_result);
        } else {
            for (const auto &err : raquest_result.error()) {
                config().errors_size++;
                printer().error(*err);
            }
        }
    }

    if (config().errors_size != 0) {
        printer().error_footer(config().errors_size);
        return 1;
    }

    return 0;
}
