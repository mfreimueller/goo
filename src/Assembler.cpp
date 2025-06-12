//
// Created by michael on 10.06.25.
//

#include "Assembler.h"

#include <format>

namespace goo {
    std::string Assembler::execute(const std::vector<Stmt *> &statements) {
        for (auto &stmt: statements) {
            stmt->accept(this);
        }

        // After executing all commands we want to finalize our asm code by adding the exit syscall.
        // This also prevents successive calls to ::execute from creating an inconsistent state, as this exit command
        // kills the process.
        return builder.mov("rax", "60")
                .x_or("rdi", "rdi")
                .syscall()
                .build();
    }

    void Assembler::visitIncrementByte(IncrementByte *stmt) {
        const auto incGuard = std::format("incGuard{}", ++labelCounter);

        builder.add("byte [tape + rbx]", "1")
                .comment(stmt->debugInfo());
                // .cmp("byte [tape + rbx]", "0")
                // .jge(incGuard)
                // .mov("byte [tape + rbx]", "0")
                // .label(incGuard);
    }

    void Assembler::visitDecrementByte(DecrementByte *stmt) {
        const auto decGuard = std::format("decGuard{}", ++labelCounter);

        builder.sub("byte [tape + rbx]", "1")
                .comment(stmt->debugInfo());
                // .cmp("byte [tape + rbx]", "0")
                // .jge(decGuard)
                // .mov("byte [tape + rbx]", "127")
                // .label(decGuard);
    }

    void Assembler::visitIncrementPtr(IncrementPtr *stmt) {
        const auto ptrGuard = std::format("ptrGuard{}", ++labelCounter);

        // as our tape is 30.000 bytes long, we need
        // a guard to jump back to 0 in case of overflow
        builder.add("rbx", "1")
                .comment(stmt->debugInfo())
                .cmp("rbx", "29999")
                .jle(ptrGuard)
                .mov("rbx", "0")
                .label(ptrGuard);
    }

    void Assembler::visitDecrementPtr(DecrementPtr *stmt) {
        const auto ptrGuard = std::format("ptrGuard{}", ++labelCounter);

        builder.sub("rbx", "1")
                .comment(stmt->debugInfo())
                .cmp("rbx", "30000")
                .jge(ptrGuard)
                .mov("rbx", "29999")
                .label(ptrGuard);
    }

    void Assembler::visitInput(Input *stmt) {
        builder.mov("rax", "0")
                .comment(stmt->debugInfo())
                .mov("rdi", "0")
                .lea("rsi", "[tape + rbx]")
                .mov("rdx", "1")
                .syscall()
                .newLine();
    }

    void Assembler::visitOutput(Output *stmt) {
        builder.mov("rax", "1")
                .comment(stmt->debugInfo())
                .mov("rdi", "1")
                .lea("rsi", "[tape + rbx]")
                .mov("rdx", "1")
                .syscall()
                .newLine();
    }

    /// Creates the base structure of a loop and recursively calls
    /// resolve for each statement. Because it is possible to have multiple
    /// conditionals, even recursively so, we track the number of loops
    /// and use it as a label.
    void Assembler::visitConditional(Conditional *stmt) {
        const auto loopLabel = std::format("loop{}", ++labelCounter);
        const auto exitLoopLabel = loopLabel + "Exit";

        // As we need to perform the loop-condition check first, we add it before translating any of the children
        // code.
        builder.newLine()
                .label(loopLabel)
                .comment(stmt->debugInfo())
                .cmp("byte [tape + rbx]", "byte 0")
                .jle(exitLoopLabel)
                .newLine();

        for (const auto &s: stmt->stmts) {
            s->accept(this);
        }

        builder.jmp(loopLabel)
                .label(exitLoopLabel);
    }

    void Assembler::visitDebug(Debug *stmt) {
        // There is no practical way to implement the debug statement, in my opinion.
        // To debug assembler code, it is better use gdb or similar debuggers.
    }
}
