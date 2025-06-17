//
// Created by michael on 17.06.25.
//

#include <catch2/catch_test_macros.hpp>

#include "AssemblerEmulator.h"


TEST_CASE("Assembler Emulator: make sure that statements are correctly grouped", "[asmemu]") {
    AssemblerEmulator assembler;
    assembler.execute("_start:\n"
        "mov rbx, 0\n"
        "add rbx, 1\n"
        "mov r8, 3\n"
        "sub r8, rbx\n"
        "mov byte [rax + rbx], r8");
}
