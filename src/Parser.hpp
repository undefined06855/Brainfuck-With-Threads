#pragma once
#include <fstream>
#include <print>
#include <array>
#include <atomic>
#include <vector>

namespace bf {

typedef void functionWithZeroParams();
typedef void functionWithOneParam(uintptr_t);
typedef void functionWithTwoParams(uintptr_t, uintptr_t);
typedef void functionWithThreeParams(uintptr_t, uintptr_t, uintptr_t);
typedef void functionWithFourParams(uintptr_t, uintptr_t, uintptr_t, uintptr_t);
typedef void functionWithFiveParams(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t);
typedef void functionWithSixParams(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t);

class Parser {
public:
    Parser(std::string_view path);
    ~Parser();

    std::string_view m_path;
    std::ifstream m_stream;

    std::array<unsigned char, 30'000> m_data;
    unsigned int m_dataPointer;

    std::atomic_flag m_awaitingInput;

    bf::Parser* m_parent;
    unsigned int m_childCount;

    std::vector<void*> m_moduleHandles;

    void parse();
    void input(char input);

    std::vector<uintptr_t> consumeFunctionParameters(unsigned int offset);
    void encodePointer(uintptr_t pointer);
    uintptr_t decodePointer(unsigned int offset);
};

}
