#include <CLI/CLI.hpp>
#include <curl/curl.h>
#include <future>
#include <memory>
#include <string>

#include "file.hpp"
#include "printer.hpp"
#include "raquest.hpp"

#define RAQUEST_VERSION "0.0.1"

struct AppContext {
    std::vector<std::string> input_paths;
    bool recursive = false;

    int jobs = 1;

    // TODO: atomic
    int errors_size = 0;
};

int main(int argc, char **argv) {
    curl_global_init(CURL_GLOBAL_ALL);

    CLI::App app{"a command-line HTTP client"};

    AppContext ctx;

    app.set_version_flag("--version", RAQUEST_VERSION);

    app.add_option("-j, --jobs", ctx.jobs, "an upper bound for concurrency")
        ->capture_default_str();
    ;

    auto cli_output_modifiers = app.add_option_group("output_modifiers");
    cli_output_modifiers->add_flag(
        "-v, --verbose", printer().verbose_,
        "print potentially unnessesary console output");
    cli_output_modifiers->add_flag("-q, --quiet", printer().quiet_,
                                   "only print critical error output");

    app.add_flag("--no_color", printer().color_disabled_,
                 "disable output color and text decoration");

    app.add_option("paths", ctx.input_paths,
                   "specify one or more .raq files or directories")
        ->required()
        ->check(CLI::ExistingPath)
        ->expected(-1, 1);

    app.add_flag("-r,--recursive", ctx.recursive,
                 "recursively search directories for .raq files")
        ->capture_default_str();

    CLI11_PARSE(app, argc, argv);

    auto input_filenames = collect_raq_files(ctx.input_paths, ctx.recursive);

    if (!input_filenames.has_value()) [[unlikely]] {
        std::cerr << "no input filenames error\n";
        std::exit(EXIT_FAILURE);
    }

    std::counting_semaphore<> threads_sem(ctx.jobs);
    std::vector<std::future<
        std::expected<CurlResponse, std::vector<std::unique_ptr<Error>>>>>
        futures;
    for (const auto &filename : input_filenames.value()) {
        futures.emplace_back(
            std::async(std::launch::async, [&threads_sem, filename]() {
                threads_sem.acquire();
                auto run_output = Raquest(filename).run();
                threads_sem.release();
                return run_output;
            }));
    }

    for (auto &fut : futures) {
        std::expected<CurlResponse, std::vector<std::unique_ptr<Error>>>
            raquest_result = fut.get();
        if (raquest_result.has_value()) {
            printer().response(*raquest_result);
        } else {
            for (const auto &err : raquest_result.error())
                printer().error(*err);
        }
    }

    curl_global_cleanup();

    if (ctx.errors_size != 0) {
        printer().error_footer(ctx.errors_size);
        return 1;
    }

    return 0;
}
