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
    Interpreter::Interpreter(Reporter &reporter): Phase(reporter), tapePtr(0) {
        tape = new unsigned char[TAPE_SIZE];
        memset(tape, 0, TAPE_SIZE);
    }

    Interpreter::~Interpreter() {
        delete[] tape;
        tape = nullptr;

        tapePtr = 0;
    }

    std::shared_ptr<Payload> Interpreter::run(const std::shared_ptr<Payload> payload) {
        // soft reset state to allow for reuse
        output = "";

        const auto stmtPayload = std::static_pointer_cast<StmtPayload>(payload);

        interpret(stmtPayload->stmts);

        return std::make_shared<StringPayload>(StringPayload { .value = output });
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
        tape[tapePtr]++;
    }

    void Interpreter::visitDecrementByte(DecrementByte *stmt) {
        tape[tapePtr]--;
    }

    void Interpreter::visitIncrementPtr(IncrementPtr *stmt) {
        tapePtr++;

        if (tapePtr >= TAPE_SIZE) {
            reporter.warning(stmt->line, stmt->column,
                "Attempted to move the tape pointer beyond the bounds of 30,000. Reset to 0.");
            tapePtr = 0;
        }
    }

    void Interpreter::visitDecrementPtr(DecrementPtr *stmt) {
        tapePtr--;

        if (tapePtr < 0) {
            reporter.warning(stmt->line, stmt->column,
                "Attempted to move the tape pointer below 0. Reset to 29,999.");
            tapePtr = TAPE_SIZE - 1;
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
        output += tape[tapePtr];
    }

    void Interpreter::visitConditional(Conditional *stmt) {
        while (tape[tapePtr] != 0 && !reporter.hasError()) {
            interpret(stmt->stmts);
        }
    }

    void Interpreter::visitDebug(Debug *stmt) {
        output += std::format("DEBUG: line = {}, column = {}, ptr = {}, tape = ", stmt->line, stmt->column, tapePtr);

        for (int idx = 0; idx < TAPE_SIZE; idx++) {
            if (tape[idx] != 0) {
                output += std::format("[{} = {}]", idx, tape[idx]);
            }
        }

        output += "\n";
    }


} // goo
