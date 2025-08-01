//
// Created by michael on 10.06.25.
//

#include "Interpreter.h"

#include <cstring>
#include <format>
#include <iostream>
#include <ostream>
#include <utility>
#include <unistd.h>

#include "Payload.h"
#include "Pipeline.h"
#include "Reporter.h"

#define TAPE_SIZE 30000

namespace goo {
    Interpreter::Interpreter(Reporter &reporter, std::ostream &out): Phase(reporter), tapePtr(0), out(out) {
        tape = new char[TAPE_SIZE];
        memset(tape, 0, TAPE_SIZE);
    }

    Interpreter::~Interpreter() {
        delete[] tape;
        tape = nullptr;

        tapePtr = 0;
    }

    std::shared_ptr<Payload> Interpreter::run(const std::shared_ptr<Payload> payload) {
        const auto stmtPayload = std::static_pointer_cast<StmtPayload>(payload);

        interpret(stmtPayload->stmts);

        return nullptr;
    }

    void Interpreter::interpret(const std::vector<std::shared_ptr<Stmt>> &stmts) {
        for (const auto &stmt : stmts) {
            if (stmt != nullptr) {
                stmt->accept(this);

                if (reporter.hasError()) return;
            }
        }
    }

    void Interpreter::visitIncrementByte(IncrementByte *stmt) {
        tape[tapePtr] += stmt->count;
        if (tape[tapePtr] < 0) {
            tape[tapePtr] += 128;
        }
    }

    void Interpreter::visitDecrementByte(DecrementByte *stmt) {
        tape[tapePtr] -= stmt->count;
        if (tape[tapePtr] < 0) {
            tape[tapePtr] = 128 + tape[tapePtr];
        }
    }

    void Interpreter::visitIncrementPtr(IncrementPtr *stmt) {
        tapePtr += stmt->count;

        if (tapePtr >= TAPE_SIZE) {
            reporter.warning(stmt->line, stmt->column,
                "Attempted to move the tape pointer beyond the bounds of 30,000. Wrapping back to 0.");
            tapePtr -= TAPE_SIZE;
        }
    }

    void Interpreter::visitDecrementPtr(DecrementPtr *stmt) {
        tapePtr -= stmt->count;

        if (tapePtr < 0) {
            reporter.warning(stmt->line, stmt->column,
                "Attempted to move the tape pointer below 0. Wrapping back to 29,999.");
            tapePtr = TAPE_SIZE + tapePtr;
        }
    }

    void Interpreter::visitInput(Input *stmt) {
        char buffer[1];
        if (read(STDIN_FILENO, buffer, 1) == -1) {
            reporter.error(stmt->line, stmt->column,
                "Failed to read user input.");
        }

        tape[tapePtr] = buffer[0];
    }

    void Interpreter::visitOutput(Output *stmt) {
        out << std::to_string(tape[tapePtr]) << std::flush;
    }

    void Interpreter::visitConditional(Conditional *stmt) {
        while (tape[tapePtr] != 0 && !reporter.hasError()) {
            interpret(stmt->stmts);
        }
    }

    void Interpreter::visitDebug(Debug *stmt) {
        out << std::format("DEBUG: line = {}, column = {}, ptr = {}, tape = ", stmt->line, stmt->column, tapePtr);

        for (int idx = 0; idx < TAPE_SIZE; idx++) {
            if (tape[idx] != 0) {
                std::cout << std::format("[{} = {}]", idx, tape[idx]);
            }
        }

        out << std::endl;
    }

    void Interpreter::visitReset(Reset *stmt) {
        tape[tapePtr] = stmt->initialValue;
    }

    void Interpreter::visitTransfer(Transfer *stmt) {
        int copyAddr = tapePtr + stmt->offset;
        tape[copyAddr] += tape[tapePtr];
        tape[tapePtr] = 0;

        if (tape[copyAddr] < 0) {
            tape[copyAddr] += 128;
        }
    }

    void Interpreter::visitMultiply(Multiply *stmt) {
        const int copyAddr = tapePtr + stmt->offset;
        tape[copyAddr] += stmt->times * (tape[tapePtr] + stmt->count);

        if (tape[copyAddr] < 0) {
            tape[copyAddr] += 128;
        }
    }


} // goo
