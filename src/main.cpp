#include "parser.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "usage: raquest <file.raq>" << std::endl;
        return 1;
    }

    try {
        Request request = Parser::parse_file(argv[1]);
        request.execute();
    } catch (const std::exception& ex) {
        std::cerr << "error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
