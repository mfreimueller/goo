//
// Created by michael on 10.06.25.
//

#include "Interpreter.h"

#include <cstring>
#include <iostream>
#include <ostream>
#include <utility>
#include <unistd.h>

#define TAPE_SIZE 30000

namespace goo {
    Interpreter::Interpreter(Reporter &reporter): tapePtr(0), reporter(reporter) {
        tape = new unsigned char[TAPE_SIZE];
        memset(tape, 0, TAPE_SIZE);
    }

    Interpreter::~Interpreter() {
        delete[] tape;
        tape = nullptr;

        tapePtr = 0;
    }

    void Interpreter::interpret(const std::vector<Stmt *>& statements) {
        for (const auto &stmt : statements) {
            if (stmt != nullptr) {
                stmt->accept(this);

                if (reporter.hasError()) break;
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
        std::cout << tape[tapePtr];
    }

    void Interpreter::visitConditional(Conditional *stmt) {
        while (tape[tapePtr] != 0 && !reporter.hasError()) {
            interpret(stmt->stmts);
        }
    }

    void Interpreter::visitDebug(Debug *stmt) {
        std::cout << "DEBUG: line = " << std::to_string(stmt->line) << ", column = " << std::to_string(stmt->column) << ", ptr = " << std::to_string(tapePtr) << ", tape = ";

        for (int idx = 0; idx < TAPE_SIZE; idx++) {
            if (tape[idx] != 0) {
                std::cout << "[" << std::to_string(idx) << " = " << std::to_string(tape[idx]) << "]";
            }
        }

        std::cout << std::endl;
    }


} // goo
