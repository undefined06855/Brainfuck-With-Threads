#include "Parser.hpp"
#include <filesystem>

int main(int argc, char** argv) {
    g_mainThreadID = std::this_thread::get_id();

    if (argc < 2) {
        std::println("Please provide an input file!");
        std::println("Usage: bf2 <input>");
        return 0;
    }

    if (argc > 2) {
        std::println("Ignoring extra parameters given!");
    }

    const char* path = argv[1];

    if (!std::filesystem::exists(path)) {
        std::println("Path does not exist!");
        return 0;
    }

    std::println("Parsing file {}...", path);

    auto parser = std::make_shared<bf::Parser>(path);
    parser->parse();
}
