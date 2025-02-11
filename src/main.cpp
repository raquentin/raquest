#include <CLI/CLI.hpp>
#include <curl/curl.h>
#include <future>
#include <memory>
#include <string>

#include "app_context.hpp"
#include "file.hpp"
#include "printer.hpp"
#include "raquest.hpp"

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
        "-v, --verbose", ctx.verbose,
        "print potentially unnessesary console output");
    cli_output_modifiers->add_flag("-q, --quiet", ctx.quiet,
                                   "only print critical error output");

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
        // TODO: panic
    }

    auto raquests = std::vector<Raquest *>{};
    raquests.reserve(input_filenames.value().size());
    for (const auto filename : input_filenames.value()) {
        Raquest raq(filename);
        raquests.push_back(&raq);
    }

    if (raquests.empty()) {
        // TODO: error
    }

    std::counting_semaphore<> threads_sem(ctx.jobs);
    std::vector<std::future<std::vector<std::unique_ptr<Error>>>> futures;
    for (Raquest *raq : raquests) {
        futures.emplace_back(
            std::async(std::launch::async, [raq]() { return raq->run(); }));
    }

    for (auto &fut : futures) {
        fut.get();
    }

    curl_global_cleanup();

    if (ctx.errors_size != 0) {
        print_final_error_footer(ctx.errors_size);
        return 1;
    }

    return 0;
}
