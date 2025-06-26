#include "Parser.hpp"
#include <vector>
#include <memory>
#include <thread>

#ifdef ALLOW_SYSCALLS
// #include <sys/syscall.h> // for constants
#include <unistd.h>
#endif

bf::Parser::Parser(std::string_view path)
    : m_path(path)
    , m_stream()

    , m_data({})
    , m_dataPointer(0)

    , m_awaitingInput()

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
                    m_parent->m_awaitingInput.wait(false);
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
                    m_awaitingInput.test_and_set();
                    m_awaitingInput.notify_all();
                    m_awaitingInput.wait(true);
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
                auto sizeOfPointer = sizeof(void*);
                
                // parse parameters
                std::vector<uintptr_t> params = {};

                unsigned int parsedParams = 0;
                unsigned int offset = m_dataPointer + 2;
                while (parsedParams < count) {
                    if (offset + 1 >= m_data.size()) {
                        std::println("Syscall not given enough space for varargs!!");
                        return;
                    }

                    auto type = m_data[offset++];
                    switch (type) {
                        case 1: {
                            // normal integer, very nice
                            if (offset + 1 >= m_data.size()) {
                                std::println("Syscall not given enough space for varargs!!");
                                return;
                            }

                            params.push_back(m_data[offset++]);
                            break;
                        }

                        case 2: {
                            // pointer!
                            if (offset + sizeOfPointer >= m_data.size()) {
                                std::println("Syscall not given enough space for varargs!!");
                                return;
                            }

                            uintptr_t result = 0x0;
                            unsigned int shift = (sizeOfPointer * 8) - 8;
                            for (int i = 0; i < sizeOfPointer; i++) {
                                result += (uintptr_t)m_data[offset++] << shift;
                                shift -= 8;
                            }

                            params.push_back(result);
                            break;
                        }

                        default: {
                            // unknown type
                            std::println("Unknown syscall parameter type: {}!!", type);
                            return;
                        }
                    }

                    parsedParams++;
                }

                unsigned char ret = 0x69;

#ifdef ALLOW_SYSCALLS
                switch (params.size()) {
                    case 0: ret = syscall(number); break;
                    case 1: ret = syscall(number, params[0]); break;
                    case 2: ret = syscall(number, params[0], params[1]); break;
                    case 3: ret = syscall(number, params[0], params[1], params[2]); break;
                    case 4: ret = syscall(number, params[0], params[1], params[2], params[3]); break;
                    case 5: ret = syscall(number, params[0], params[1], params[2], params[3], params[4]); break;
                    case 6: ret = syscall(number, params[0], params[1], params[2], params[3], params[4], params[5]); break;
                }
#else
                std::println("Syscall {} not available on platform! params ({}):", number, count);
                for (int i = 0; i < count; i++) {
                    std::println("    {}: {:#x}", i, params[i]);
                }
#endif
                m_data[m_dataPointer] = ret;

                break;
            }

            case '&': {
                // address of
                // returns actual memory address of current cell
                // uses next sizeof(ptr) bytes

                auto sizeOfPointer = sizeof(void*);

                if (m_dataPointer + sizeOfPointer >= m_data.size()) {
                    std::println("Addressof not given enough space for return!");
                    return;
                }

                uintptr_t ptr = (uintptr_t)(&m_data[m_dataPointer]);

                unsigned int shift = (sizeOfPointer * 8) - 8;
                for (int i = 0; i < sizeOfPointer; i++) {
                    m_data[m_dataPointer + i] = (ptr >> shift) & 0xff;
                    shift -= 8;
                }

                break;
            }
        }
    }
}

// note that this is called from inside the thread
void bf::Parser::input(char input) {
    m_data[m_dataPointer] = input;
    m_awaitingInput.clear();
    m_awaitingInput.notify_all();
}
