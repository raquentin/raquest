#pragma once

#include <string>
#include <vector>

#define RAQUEST_VERSION "0.0.1"

struct AppContext {
    bool verbose = false;
    bool quiet = false;

    std::vector<std::string> input_paths;
    bool recursive = false;

    int jobs = 1;

    // TODO: atomic
    int errors_size = 0;
};
