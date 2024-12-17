#pragma once

#include "errors/error.hpp"
#include <memory>
#include <mutex>
#include <vector>

class ErrorManager {
public:
    // Adds an error to the collection in a thread-safe manner
    void add_error(std::shared_ptr<Error> error);

    // Retrieves all collected errors
    std::vector<std::shared_ptr<Error>> get_errors() const;

    // Prints all collected errors with headers and footers
    void print_all_errors() const;

private:
    mutable std::mutex mutex_; // Mutex to protect access to errors_
    std::vector<std::shared_ptr<Error>> errors_;
};
