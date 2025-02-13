#include <string>
#include <vector>

struct Config {
    std::vector<std::string> input_paths;
    bool recursive = false;

    bool quiet = 0;
    bool verbose = 0;
    bool no_color = 0;

    int jobs = 1;
    int retries = 2;

    int errors_size = 0;
};

Config &config();
