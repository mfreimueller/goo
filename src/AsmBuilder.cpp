//
// Created by michael on 11.06.25.
//

#include "AsmBuilder.h"

#include <format>

namespace goo {
    StringAsmBuilder::StringAsmBuilder() {
        code = "bits 64\n\n";
        code += "section .bss\n\n";
        code += "\ttape: resb 30000\n\n";

        code += "\tglobal _start\n\n";

        code += "section .text\n\n";

        label("_start");
        mov("rbx", "0");
    }

    std::string StringAsmBuilder::build() const {
        return code;
    }

    AsmBuilder& StringAsmBuilder::mov(std::string dest, std::string src) {
        code += std::format("\n\tmov {}, {}", dest, src);
        return *this;
    }

    AsmBuilder &StringAsmBuilder::label(std::string name) {
        code += std::format("\n{}:", name);
        return *this;
    }

    AsmBuilder &StringAsmBuilder::x_or(std::string dest, std::string src) {
        code += std::format("\n\txor {}, {}", dest, src);
        return *this;
    }

    AsmBuilder &StringAsmBuilder::syscall() {
        code += "\n\tsyscall";
        return *this;
    }

    AsmBuilder &StringAsmBuilder::add(std::string dest, std::string src) {
        code += std::format("\n\tadd {}, {}", dest, src);
        return *this;
    }

    AsmBuilder &StringAsmBuilder::sub(std::string dest, std::string src) {
        code += std::format("\n\tsub {}, {}", dest, src);
        return *this;
    }

    AsmBuilder &StringAsmBuilder::mul(std::string src) {
        code += std::format("\n\tmul {}", src);
        return *this;
    }

    AsmBuilder &StringAsmBuilder::lea(std::string dest, std::string src) {
        code += std::format("\n\tlea {}, {}", dest, src);
        return *this;
    }

    AsmBuilder &StringAsmBuilder::cmp(std::string dest, std::string src) {
        code += std::format("\n\tcmp {}, {}", dest, src);
        return *this;
    }

    AsmBuilder &StringAsmBuilder::jmp(std::string label) {
        code += std::format("\n\tjmp {}", label);
        return *this;
    }

    AsmBuilder &StringAsmBuilder::jg(std::string label) {
        code += std::format("\n\tjg {}", label);
        return *this;
    }

    AsmBuilder &StringAsmBuilder::jle(std::string label) {
        code += std::format("\n\tjle {}", label);
        return *this;
    }

    AsmBuilder &StringAsmBuilder::jge(std::string label) {
        code += std::format("\n\tjge {}", label);
        return *this;
    }

    AsmBuilder &StringAsmBuilder::comment(std::string comment) {
        code += std::format(" ; {}", comment);
        return *this;
    }

    AsmBuilder &StringAsmBuilder::newLine() {
        code += "\n";
        return *this;
    }
} // goo