#include "Parser.hpp"
#include <span>
#include <utility>
#include <memory>

#ifdef ALLOW_SYSCALLS
// #include <sys/syscall.h> // for constants
#include <unistd.h>
#endif

bf::Parser::Parser(std::string_view path)
    : m_path(path)
    , m_stream()

    , m_data({})
    , m_dataPointer(0)

    , m_awaitingInput(false)

    , m_parent(nullptr)
    , m_childCount(0) {
    
    m_stream.open(path.data());
    if (!m_stream) {
        std::println("Could not open path!!");
        return;
    }
}

void bf::Parser::parse() {
    while (true) {
        char next;
        m_stream.get(next);

        if (!m_stream) {
            std::println("\nFinished, end of file.");
            if (m_parent) {
                std::println("...but this is a thread!!");
            }
            return;
        }

        switch (next) {
            case '<': {
                if (m_dataPointer == 0) {
                    std::println("Data pointer subtracted below zero!!");
                    return;
                }

                m_dataPointer--;
                break;
            }

            case '>': {
                if (m_dataPointer == m_data.size()) {
                    std::println("Data pointer incremented above {}!!", m_data.size());
                    return;
                }

                m_dataPointer++;
                break;
            }

            case '+': {
                m_data[m_dataPointer]++;
                break;
            }

            case '-': {
                m_data[m_dataPointer]--;
                break;
            }

            case '.': {
                if (m_parent) {
                    // output to parent if this is a thread
                    while (!m_parent->m_awaitingInput) { /* spin */ }
                    m_parent->input(m_data[m_dataPointer]);
                } else {
                    // output to console if this is main thread
                    std::print("{}", static_cast<char>(m_data[m_dataPointer]));
                }

                break;
            }

            case ',': {
                if (m_childCount == 0) {
                    // TODO: take input from console if no threads
                    std::println("Taking input from console is not implemented yet!");
                } else {
                    // wait for input from child threads
                    m_awaitingInput = true;
                    while (m_awaitingInput) { /* spin */ }
                }

                break;
            }

            case '[': {
                if (m_data[m_dataPointer] != 0) continue;

                // seek to next matching closing bracket where depth is zero,
                // incrementing depth by every opening bracket
                
                unsigned int depth = 1;
                
                do {
                    m_stream.get(next);

                    if (!m_stream) {
                        std::println("Found EOF while trying to match closing bracket!!");
                        return;
                    }

                    if (next == '[') depth++;
                    if (next == ']') depth--;
                } while (next != ']' && depth != 0);

                break;
            }

            case ']': {
                if (m_data[m_dataPointer] == 0) continue;

                // seek to previous matching opening bracket where depth is
                // zero, incrementing depth by every closing bracket

                unsigned int depth = 1;

                do {
                    m_stream.seekg(-1, std::ios::cur);
                    m_stream.get(next);
                    m_stream.seekg(-1, std::ios::cur);

                    if (!m_stream) {
                        std::println("Found beginning of file while trying to match opening bracket!!");
                        return;
                    }

                    if (next == ']') depth++;
                    if (next == '[') depth--;
                } while (next != '[' && depth != 0);

                m_stream.seekg(1, std::ios::cur);

                break;
            }

            // custom stuff
            case '{': {
                // begin thread
                auto start = m_stream.tellg();
                unsigned char param = m_data[m_dataPointer];

                std::thread([start, param, this]{
                    auto child = std::make_shared<bf::Parser>(m_path);
                    child->m_data[0] = param; // pass in param
                    child->m_parent = this;
                    child->m_stream.seekg(start);
                    child->parse();
                }).detach();

                m_childCount++;

                // then seek to end of the thread, matching closing bracket
                // where depth is zero

                unsigned int depth = 1;
                
                do {
                    m_stream.get(next);

                    if (!m_stream) {
                        std::println("Found EOF while trying to match closing thread bracket!!");
                        return;
                    }

                    if (next == '{') depth++;
                    if (next == '}') depth--;
                } while (next != '}' && depth != 0);

                break;
            }

            case '}': {
                // exit this thread
                if (!m_parent) {
                    // this is main thread!
                    std::println("Found closing thread bracket but this is main thread!!");
                    return;
                }

                m_parent->m_childCount--;

                return;
            }

            case '!': {
                // syscall!
                if (m_dataPointer + 1 >= m_data.size()) {
                    std::println("Syscall not given enough space for parameters!!");
                    return;
                }

                unsigned char number = m_data[m_dataPointer];
                unsigned char count = m_data[m_dataPointer + 1];

                if (m_dataPointer + 2 + count > m_data.size()) {
                    std::println("Syscall not given enough space for arguments!");
                    return;
                }

                auto params = std::span(m_data).subspan(m_dataPointer + 2, count);

                unsigned char ret;

#ifdef ALLOW_SYSCALLS
                switch (params.size()) {
                    case 0: ret = syscall(number);
                    case 1: ret = syscall(number, params[0]);
                    case 2: ret = syscall(number, params[0], params[1]);
                    case 3: ret = syscall(number, params[0], params[1], params[2]);
                    case 4: ret = syscall(number, params[0], params[1], params[2], params[3]);
                    case 5: ret = syscall(number, params[0], params[1], params[2], params[3], params[4]);
                    case 6: ret = syscall(number, params[0], params[1], params[2], params[3], params[4], params[5]);
                }
#else
                std::println("syscall {} not available on platform! params ({}):", number, count);
                for (auto param : params) {
                    std::println("    {}", param);
                }
                ret = 0;
#endif
                m_data[m_dataPointer] = ret;
            }
        }
    }
}

void bf::Parser::input(char input) {
    if (!m_awaitingInput) {
        // should check if awaiting input before calling input
        // would only get here if some weird timing stuff happens
        std::unreachable();
        return;
    }

    m_data[m_dataPointer] = input;
    m_awaitingInput = false;
}
