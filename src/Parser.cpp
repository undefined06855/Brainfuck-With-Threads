#include "Parser.hpp"
#include <vector>
#include <memory>
#include <thread>

#ifdef IS_LINUX
// #include <sys/syscall.h> // for constants
#include <unistd.h> // for syscalls
#include <dlfcn.h> // for dlopen
#else
#include <windows.h> // for getmodulehandle/getprocaddress
#endif

bf::Parser::Parser(std::string_view path)
    : m_path(path)
    , m_stream()

    , m_data({})
    , m_dataPointer(0)

    , m_awaitingInput()

    , m_parent(nullptr)
    , m_childCount(0)
    
    , m_moduleHandles({}) {
    m_stream.open(path.data());
    if (!m_stream) {
        std::println("Could not open path!!");
        return;
    }
}

bf::Parser::~Parser() {
    // free all handles cross-platform

    for (auto handle : m_moduleHandles) {
#ifdef IS_LINUX
        dlclose(handle);
#else
        FreeLibrary((HMODULE)handle);
#endif
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
                    // (wait for flag to be set)
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
                    // (wait for flag to be cleared)
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
                
                unsigned char number = m_data[m_dataPointer];
                unsigned char ret = 0x0;

                auto params = consumeFunctionParameters(m_dataPointer + 1);

#ifdef IS_LINUX
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
                std::println("Syscall {} not available on platform! params ({}):", number, params.size());
                for (int i = 0; i < params.size(); i++) {
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

                encodePointer((uintptr_t)&m_data[m_dataPointer]);

                break;
            }

            case '$': {
                // call c function
                auto sizeOfPointer = sizeof(void*);

                if (m_dataPointer + sizeOfPointer + sizeOfPointer >= m_data.size()) {
                    std::println("Function call not given enough space for parameters!!");
                    return;
                }


                // do we actually trust these? probably not but uh it doesnt
                // really matter
                const char* library = (char*)decodePointer(m_dataPointer);
                const char* function = (char*)decodePointer(m_dataPointer + sizeof(void*));

                auto params = consumeFunctionParameters(m_dataPointer + sizeof(void*) + sizeof(void*));

                std::println("Calling function {} from library {} with {} params", function, library, params.size());

#ifdef IS_LINUX
                dlerror();

                auto handle = dlopen(library, RTLD_LAZY);
                if (auto err = dlerror()) {
                    std::println("Failed to open library {}!! ({})", library, err);
                    return;
                }
                m_moduleHandles.push_back(handle);

                void* address = dlsym(handle, function);
                if (auto err = dlerror()) {
                    std::println("Failed to find function {}!! ({})", function, err);
                }


#else
                auto handle = LoadLibraryA(library);
                if (!handle) {
                    std::println("Failed to open library {}!!", library);
                    return;
                }
                m_moduleHandles.push_back((void*)handle);

                void* address = (void*)GetProcAddress(handle, function);
                if (!address) {
                    std::println("Failed to find function {}!!", function);
                    return;
                }
#endif

                // call address (void*)
                switch (params.size()) {
                    case 0: ((functionWithZeroParams*)(address))(); break;
                    case 1: ((functionWithOneParam*)(address))(params[0]); break;
                    case 2: ((functionWithTwoParams*)(address))(params[0], params[1]); break;
                    case 3: ((functionWithThreeParams*)(address))(params[0], params[1], params[2]); break;
                    case 4: ((functionWithFourParams*)(address))(params[0], params[1], params[2], params[3]); break;
                    case 5: ((functionWithFiveParams*)(address))(params[0], params[1], params[2], params[3], params[4]); break;
                    case 6: ((functionWithSixParams*)(address))(params[0], params[1], params[2], params[3], params[4], params[5]); break;
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

std::vector<uintptr_t> bf::Parser::consumeFunctionParameters(unsigned int offset) {
    if (offset + 1 >= m_data.size()) {
        std::println("Not given enough space for parameters!!");
        return {};
    }

    unsigned char count = m_data[offset++];
    auto sizeOfPointer = sizeof(void*);
    
    // parse parameters
    std::vector<uintptr_t> params = {};

    unsigned int parsedParams = 0;
    while (parsedParams < count) {
        if (offset + 1 >= m_data.size()) {
            std::println("Not given enough space for varargs!!");
            return {};
        }

        auto type = m_data[offset++];
        switch (type) {
            case 1: {
                // normal integer, very nice
                if (offset + 1 >= m_data.size()) {
                    std::println("Not given enough space for varargs!!");
                    return {};
                }

                params.push_back(m_data[offset++]);
                break;
            }

            case 2: {
                // pointer!
                if (offset + sizeOfPointer >= m_data.size()) {
                    std::println("Not given enough space for varargs!!");
                    return {};
                }

                params.push_back(decodePointer(offset));
                offset += sizeOfPointer;
                break;
            }

            default: {
                // unknown type
                std::println("Unknown parameter type: {}!!", type);
                return {};
            }
        }

        parsedParams++;
    }

    return params;
}

void bf::Parser::encodePointer(uintptr_t pointer) {
    unsigned int shift = (sizeof(void*) * 8) - 8;
    for (int i = 0; i < sizeof(void*); i++) {
        m_data[m_dataPointer + i] = (pointer >> shift) & 0xff;
        shift -= 8;
    }
}

uintptr_t bf::Parser::decodePointer(unsigned int offset) {
    uintptr_t result = 0x0;
    unsigned int shift = (sizeof(void*) * 8) - 8;
    for (int i = 0; i < sizeof(void*); i++) {
        result += (uintptr_t)m_data[offset++] << shift;
        shift -= 8;
    }

    return result;
}
