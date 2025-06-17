//
// Created by michael on 17.06.25.
//

#include "AssemblerEmulator.h"

#include <cstdint>
#include <cstring>
#include <format>
#include <iostream>
#include <sstream>
#include "../src/Util.h"

AssemblerEmulator::AssemblerEmulator() {
    tape = new int[30000];
    memset(tape, 0, sizeof(int[30000]));
}

AssemblerEmulator::~AssemblerEmulator() {
    delete[] tape;
}

std::string AssemblerEmulator::execute(const std::string &code) {
    auto stream = std::stringstream{code};

    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty()) {
            // We cut away any comments
            auto commentLocation = line.find(';');
            if (commentLocation != std::string::npos) {
                line = line.substr(0, commentLocation);
            }

            line = goo::stripWhitespace(line);

            // as only labels end with : we track the line number
            if (line.ends_with(":")) {
                labels[line.substr(0, line.size() - 1)] = lines.size();

                // if this is the _start label, we store its location in execPtr
                if (line == "_start:") {
                    execPtr = lines.size();
                }
            } else {
                lines.push_back(line);
            }
        }
    }

    while (!hasError && execPtr < lines.size()) {
        execPtr = executeLine(execPtr);
    }

    return output;
}

int AssemblerEmulator::executeLine(const int execPtr) {
    const auto &line = lines[execPtr];

    auto cmdPos = line.find(' ');
    if (cmdPos == std::string::npos) {
        cmdPos = line.size();
    }

    const auto &cmd = line.substr(0, cmdPos);

    std::vector<std::string> args;
    if (line.size() > cmdPos) {
        auto tail = line.substr(cmdPos + 1);

        const auto commaPos = tail.find(',');
        if (commaPos != std::string::npos) {
            const auto first = goo::stripWhitespace(tail.substr(0, commaPos));
            const auto second = goo::stripWhitespace(tail.substr(commaPos + 1));

            args.push_back(first);
            args.push_back(second);
        } else {
            args.push_back(goo::stripWhitespace(tail));
        }
    }

    if (cmd == "mov")
        return mov(args, execPtr);
    if (cmd == "lea")
        return execPtr + 1;
    if (cmd == "add")
        return add(args, execPtr);
    if (cmd == "cmp")
        return cmp(args, execPtr);
    if (cmd == "jge")
        return jge(args, execPtr);
    if (cmd == "jle")
        return jle(args, execPtr);
    if (cmd == "mul")
        return mul(args, execPtr);
    if (cmd == "sub")
        return sub(args, execPtr);
    if (cmd == "syscall")
        return syscall(args, execPtr);
    if (cmd == "jmp")
        return jmp(args, execPtr);
    if (cmd == "xor")
        return execPtr + 1;

    hasError = true;
    return -1;
}

int AssemblerEmulator::mov(Params args, const int execPtr) {
    if (args.size() != 2) {
        hasError = true;
        return -1;
    }

    const auto src = getValue(args[1]);
    auto dest = getValue(args[0]);

    if (dest.ptr == nullptr) {
        hasError = true;
        return -1;
    }

    memcpy(dest.ptr, &src.value, sizeof(int));

    return execPtr + 1;
}

int AssemblerEmulator::add(Params args, const int execPtr) {
    if (args.size() != 2) {
        hasError = true;
        return -1;
    }

    const auto src = getValue(args[1]);
    auto dest = getValue(args[0]);

    if (dest.ptr == nullptr) {
        hasError = true;
        return -1;
    }

    const auto sum = src.value + dest.value;
    memcpy(dest.ptr, &sum, sizeof(int));

    return execPtr + 1;
}

int AssemblerEmulator::cmp(Params args, const int execPtr) {
    if (args.size() != 2) {
        hasError = true;
        return -1;
    }

    const auto src = getValue(args[1]);
    const auto dest = getValue(args[0]);

    if (dest.ptr == nullptr) {
        hasError = true;
        return -1;
    }

    cmpResult = dest.value - src.value;

    return execPtr + 1;
}

int AssemblerEmulator::jge(Params args, const int execPtr) {
    const auto &label = args[0];

    if (cmpResult >= 0) {
        return labels[label];
    }

    return execPtr + 1;
}

int AssemblerEmulator::jle(Params args, const int execPtr) {
    const auto &label = args[0];

    if (cmpResult <= 0) {
        return labels[label];
    }

    return execPtr + 1;
}

int AssemblerEmulator::mul(Params args, const int execPtr) {
    const auto src = getValue(args[0]);
    rax = rax * src.value;

    return execPtr + 1;
}

int AssemblerEmulator::sub(Params args, const int execPtr) {
    if (args.size() != 2) {
        hasError = true;
        return -1;
    }

    const auto src = getValue(args[1]);
    auto dest = getValue(args[0]);

    if (dest.ptr == nullptr) {
        hasError = true;
        return -1;
    }

    const auto difference = dest.value - src.value;
    memcpy(dest.ptr, &difference, sizeof(int));

    return execPtr + 1;
}

int AssemblerEmulator::jmp(Params args, int execPtr) {
    const auto &label = args[0];
    return labels[label];
}

int AssemblerEmulator::syscall(Params args, int execPtr) {
    if (rax == 1 && rdi == 1) {
        output += std::format("{}", tape[rbx]);
        return execPtr + 1;
    }

    // any other call: ignore
    return execPtr + 1;
}


void *AssemblerEmulator::getPointer(const std::string &label) {
    if (label == "rax")
        return &rax;
    if (label == "rbx")
        return &rbx;
    if (label == "rcx")
        return &rcx;
    if (label == "rdx")
        return &rdx;
    if (label == "rdi")
        return &rdi;
    if (label == "rsi")
        return &rsi;
    if (label == "r8" || label == "r8b")
        return &r8;
    if (label == "r9" || label == "r9b")
        return &r9;
    if (label == "r10" || label == "r10b")
        return &r10;

    // if we reach this pointer, we either has an invalid register or
    // refer to the tape etc
    if (label.find("[rel tape]") != std::string::npos) {
        return tape;
    } else if (label.find("[rax + rbx]") != std::string::npos) {
        return &tape[rbx];
    }

    return nullptr;
}

Value AssemblerEmulator::getValue(const std::string& label) {
    void *ptr = getPointer(label);
    int value = 0;

    // if ptr is null try to convert label to int
    if (ptr == nullptr) {
        auto strValue = label;
        if (strValue.find("byte ") != std::string::npos) {
            strValue = strValue.substr(5);
        }

        try {
            value = std::stoi(strValue);
        } catch (std::invalid_argument &e) {
            std::cerr << e.what() << std::endl;
        }
    } else {
        memcpy(&value, ptr, sizeof(int));
    }

    return Value {
        .ptr = ptr,
        .value = value
    };
}