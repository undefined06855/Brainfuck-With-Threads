#pragma once
#include <fstream>
#include <print>
#include <array>
#include <atomic>

namespace bf {

class Parser {
public:
    Parser(std::string_view path);

    std::string_view m_path;
    std::ifstream m_stream;

    std::array<unsigned char, 30'000> m_data;
    unsigned int m_dataPointer;

    std::atomic_flag m_awaitingInput;

    bf::Parser* m_parent;
    unsigned int m_childCount;

    void parse();
    void input(char input);
};

}
