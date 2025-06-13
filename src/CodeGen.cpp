//
// Created by michael on 10.06.25.
//

#include "CodeGen.h"

#include <format>

/*
 * Some notes on register usage:
 * rax stores the addresses (tape, syscalls etc.)
 * rbx stores the tape pointer and should only be used for this.
 * rdi is unused.
 * rdx is used by increment/decrement ops, therefor it is unsafe to use it in a different context.
 * r8 to r11 may be used for storing data.
 */

namespace goo {
    std::shared_ptr<Payload> CodeGen::run(const std::shared_ptr<Payload> payload) {
        // reset state of code gen to allow for reuse
        labelCounter = 0;

        const auto stmtPayload = std::static_pointer_cast<StmtPayload>(payload);

        for (const auto stmts = stmtPayload->stmts; auto &stmt: stmts) {
            if (stmt != nullptr) {
                stmt->accept(this);
            }
        }

        // After executing all commands we want to finalize our asm code by adding the exit syscall.
        // This also prevents successive calls to ::execute from creating an inconsistent state, as this exit command
        // kills the process.
        const auto code = builder->mov("rax", "60")
                .x_or("rdi", "rdi")
                .syscall()
                .build();

        return std::make_shared<StringPayload>(StringPayload{.value = code});
    }

    void CodeGen::visitIncrementByte(IncrementByte *stmt) {
        const auto incGuard = std::format("incGuard{}", ++labelCounter);

        builder->lea("rax", "[rel tape]");

        if (stmt->count == 1) {
            builder->add("byte [rax + rbx]", "1");

            if (config.debugBuild) {
                builder->comment(stmt->debugInfo());
            }

            builder->cmp("byte [rax + rbx]", "0")
                    .jge(incGuard)
                    .add("byte [rax + rbx]", "127");
        } else {
            // We simply add [tape + rbx] + moves. In case of an overflow we wrap around and end up with a negative
            // number. Then we simply add 127 to make the number positive again.
            builder->add("byte [rax + rbx]", std::to_string(stmt->count));

            if (config.debugBuild) {
                builder->comment(stmt->debugInfo());
            }

            builder->cmp("byte [rax + rbx]", "0")
                    .jge(incGuard);

            // Now rbx is smaller than 0, we had an overflow. Therefor we add 127 to move to positive values.
            builder->add("byte [rax + rbx]", "127");
        }

        builder->label(incGuard);
    }

    void CodeGen::visitDecrementByte(DecrementByte *stmt) {
        const auto decGuard = std::format("decGuard{}", ++labelCounter);

        builder->lea("rax", "[rel tape]");

        if (stmt->count == 1) {
            builder->sub("byte [rax + rbx]", "1");

            if (config.debugBuild) {
                builder->comment(stmt->debugInfo());
            }

            builder->cmp("byte [rax + rbx]", "0")
                    .jge(decGuard)
                    .mov("byte [rax + rbx]", "127");
        } else {
            // There are two possible outcomes. First, moves is smaller or equal to the value in rbx. In this case we simply
            // subtract rbx-moves. Otherwise, we calculate 127 - (moves-rbx) and store this in rbx.
            const auto underflowGuard = std::format("underflowGuard{}", labelCounter);

            builder->mov("r8b", std::to_string(stmt->count));

            if (config.debugBuild) {
                builder->comment(stmt->debugInfo());
            }

            builder->cmp("r8b", "byte [rax + rbx]")
                    .jle(underflowGuard);

            // Now rdx is larger. Therefor we subtract rbx from rdx, write 127 into rbx and subtract rdx from rbx.
            builder->sub("r8b", "byte [rax + rbx]")
                    .mov("byte [rax + rbx]", "127")
                    .label(underflowGuard);

            // In any case we must subtract rdx from rbx, therefor we either jump directly to here or 'fall' through.
            builder->sub("byte [rax + rbx]", "r8b");
        }

        builder->label(decGuard);
    }

    void CodeGen::visitIncrementPtr(IncrementPtr *stmt) {
        const auto labelCounter = ++this->labelCounter;
        const auto ptrGuard = std::format("ptrGuard{}", labelCounter);

        if (stmt->count == 1) {
            // as our tape is 30.000 bytes long, we need
            // a guard to jump back to 0 in case of overflow
            builder->add("rbx", "1");

            if (config.debugBuild) {
                builder->comment(stmt->debugInfo());
            }

            builder->cmp("rbx", "29999")
                    .jle(ptrGuard)
                    .sub("rbx", "29999");
        } else {
            // There are two possible outcomes. First, moves is smaller or equal to the value in rbx. In this case we simply
            // subtract rbx-moves. Otherwise, we calculate 29,999 - (moves-rbx) and store this in rbx.

            builder->add("rbx", std::to_string(stmt->count));

            if (config.debugBuild) {
                builder->comment(stmt->debugInfo());
            }

            builder->cmp("rbx", "29999")
                    .jle(ptrGuard);

            // Now rbx is larger than 29,999. Therefor we subtract 29,999 from rbx.
            builder->sub("rbx", "29999");
        }

        builder->label(ptrGuard);
    }

    void CodeGen::visitDecrementPtr(DecrementPtr *stmt) {
        const auto ptrGuard = std::format("ptrGuard{}", ++labelCounter);

        if (stmt->count == 1) {
            builder->sub("rbx", "1");

            if (config.debugBuild) {
                builder->comment(stmt->debugInfo());
            }

            builder->cmp("rbx", "0")
                    .jge(ptrGuard)
                    .mov("rbx", "29999");
        } else {
            // There are two possible outcomes. First, moves is smaller or equal to the value in rbx. In this case we simply
            // subtract rbx-moves. Otherwise, we calculate 29,999 - (moves-rbx) and store this in rbx.
            const auto underflowGuard = std::format("underflowGuard{}", labelCounter);

            builder->mov("rdx", std::to_string(stmt->count));

            if (config.debugBuild) {
                builder->comment(stmt->debugInfo());
            }

            builder->cmp("rdx", "rbx")
                    .jle(underflowGuard);

            // Now rdx is larger. Therefor we subtract rbx from rdx, write 29,999 into rbx and subtract rdx from rbx.
            builder->sub("rdx", "rbx")
                    .mov("rbx", "29999")
                    .label(underflowGuard);

            // In any case we must subtract rdx from rbx, therefor we either jump directly to here or 'fall' through.
            builder->sub("rbx", "rdx");
        }

        builder->label(ptrGuard);
    }

    void CodeGen::visitInput(Input *stmt) {
        builder->mov("rax", "0");

        if (config.debugBuild) {
            builder->comment(stmt->debugInfo());
        }

        builder->mov("rdi", "0")
                .lea("rsi", "[tape + rbx]")
                .mov("rdx", "1")
                .syscall()
                .newLine();
    }

    void CodeGen::visitOutput(Output *stmt) {
        builder->mov("rax", "1");

        if (config.debugBuild) {
            builder->comment(stmt->debugInfo());
        }

        builder->mov("rdi", "1")
                .lea("rsi", "[tape + rbx]")
                .mov("rdx", "1")
                .syscall()
                .newLine();
    }

    /// Creates the base structure of a loop and recursively calls
    /// resolve for each statement. Because it is possible to have multiple
    /// conditionals, even recursively so, we track the number of loops
    /// and use it as a label.
    void CodeGen::visitConditional(Conditional *stmt) {
        const auto loopLabel = std::format("loop{}", ++labelCounter);
        const auto exitLoopLabel = loopLabel + "Exit";

        // As we need to perform the loop-condition check first, we add it before translating any of the children
        // code.
        builder->newLine()
                .label(loopLabel);

        if (config.debugBuild) {
            builder->comment(stmt->debugInfo());
        }

        builder->lea("rax", "[rel tape]")
                .cmp("byte [rax + rbx]", "byte 0")
                .jle(exitLoopLabel)
                .newLine();

        for (const auto &s: stmt->stmts) {
            s->accept(this);
        }

        builder->jmp(loopLabel)
                .label(exitLoopLabel);
    }

    void CodeGen::visitDebug(Debug *stmt) {
        builder->mov("r8b", "byte [tape + rbx]");
    }

    void CodeGen::visitReset(Reset *stmt) {
        builder->
                lea("rax", "[rel tape]")
                .mov("byte [rax + rbx]", std::to_string(stmt->initialValue));

        if (config.debugBuild) {
            builder->comment(stmt->debugInfo());
        }
    }

    void CodeGen::visitTransfer(Transfer *stmt) {
        // We store the original pointer in rax
        builder->mov("r8", "rbx");

        if (config.debugBuild) {
            builder->comment(stmt->debugInfo());
        }

        // Then we (ab)use our pointer inc/dec logic to move the pointer to the proper offset
        // First we move the pointer to where we want to copy from
        if (stmt->subOffset > 0) {
            const auto incPtr = new IncrementPtr(stmt->column, stmt->line, stmt->subOffset);
            visitIncrementPtr(incPtr);
            delete incPtr;
        } else if (stmt->subOffset < 0) {
            const auto decPtr = new DecrementPtr(stmt->column, stmt->line, -stmt->subOffset);
            visitDecrementPtr(decPtr);
            delete decPtr;
        }

        // Then we move the value from rbx to r10 to store it, while we do the same for the value we want to copy to
        builder->mov("r10", "rbx");

        if (stmt->subOffset != 0) {
            builder->mov("rbx", "r8"); // Reset to the original ptr location
        }

        if (stmt->addOffset > 0) {
            const auto incPtr = new IncrementPtr(stmt->column, stmt->line, stmt->addOffset);
            visitIncrementPtr(incPtr);
            delete incPtr;
        } else if (stmt->addOffset < 0) {
            const auto decPtr = new DecrementPtr(stmt->column, stmt->line, -stmt->addOffset);
            visitDecrementPtr(decPtr);
            delete decPtr;
        }

        // Now we want to do the following tape[rbx] += tape[r10]
        const auto incGuard = std::format("incGuard{}", ++labelCounter);

        builder->lea("rax", "[rel tape]")
                .mov("r9b", "byte [rax + r10]")
                .add("byte [rax + rbx]", "r9b")
                .cmp("byte [rax + rbx]", "0")
                .jge(incGuard);

        // Now rbx is smaller than 0, we had an overflow. Therefor we add 127 to move to positive values.
        builder->add("byte [rax + rbx]", "127");

        // Finally we reset our tape pointer to the original value, as a copy statement doesn't move the pointer.
        builder->label(incGuard)
                .mov("rbx", "r8");
    }
}
