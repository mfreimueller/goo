//
// Created by michael on 10.06.25.
//

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Stmt.h"

namespace goo {

class Interpreter final : public Visitor {
    char *tape;
    int tapePtr;

public:
    Interpreter();
    ~Interpreter() override;

    void interpret(const std::vector<Stmt *>& statements);

private:
    void visitIncrementByte(IncrementByte *stmt) override;

    void visitDecrementByte(DecrementByte *stmt) override;

    void visitIncrementPtr(IncrementPtr *stmt) override;

    void visitDecrementPtr(DecrementPtr *stmt) override;

    void visitConditional(Conditional *stmt) override;

    void visitOutput(Output *stmt) override;

    void visitInput(Input *stmt) override;

    void visitDebug(Debug *stmt) override;
};

} // goo

#endif //INTERPRETER_H
