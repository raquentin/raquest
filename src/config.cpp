#include "config.hpp"

Config &config() {
    static Config instance;
    return instance;
}
