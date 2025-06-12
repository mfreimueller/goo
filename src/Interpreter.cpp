//
// Created by michael on 10.06.25.
//

#include "Interpreter.h"

#include <cstring>
#include <iostream>
#include <ostream>
#include <unistd.h>

#define TAPE_SIZE 30000

namespace goo {
    Interpreter::Interpreter(): tapePtr(0) {
        tape = new char[TAPE_SIZE];
        memset(tape, 0, TAPE_SIZE);
    }

    Interpreter::~Interpreter() {
        delete[] tape;
        tape = nullptr;

        tapePtr = 0;
    }

    void Interpreter::interpret(const std::vector<Stmt *>& statements) {
        for (const auto &stmt : statements) {
            stmt->accept(this);
        }
    }

    void Interpreter::visitIncrementByte(IncrementByte *stmt) {
        tape[tapePtr]++;
        if (tape[tapePtr] < 0) {
            tape[tapePtr] = 0;
        }
    }

    void Interpreter::visitDecrementByte(DecrementByte *stmt) {
        tape[tapePtr]--;
        if (tape[tapePtr] < 0) {
            tape[tapePtr] = 127;
        }
    }

    void Interpreter::visitIncrementPtr(IncrementPtr *stmt) {
        tapePtr++;

        if (tapePtr >= TAPE_SIZE) {
            std::cerr << "tape ptr reset to 0 (overflow)" << std::endl;
            tapePtr = 0;
        }
    }

    void Interpreter::visitDecrementPtr(DecrementPtr *stmt) {
        tapePtr--;

        if (tapePtr < 0) {
            std::cerr << "tape ptr reset to 29999 (underflow)" << std::endl;
            tapePtr = TAPE_SIZE - 1;
        }
    }

    void Interpreter::visitInput(Input *stmt) {
        char buffer[1];
        if (read(STDIN_FILENO, buffer, 1) == -1) {
            // TODO: handle error
        }

        tape[tapePtr] = buffer[0];
    }

    void Interpreter::visitOutput(Output *stmt) {
        std::cout << tape[tapePtr];
    }

    void Interpreter::visitConditional(Conditional *stmt) {
        while (tape[tapePtr] != 0) {
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
