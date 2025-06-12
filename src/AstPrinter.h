//
// Created by michael on 10.06.25.
//

#ifndef ASTPRINTER_H
#define ASTPRINTER_H
#include "Stmt.h"

namespace brainlove {

class AstPrinter final : public Visitor {
    int level = 0;

public:
    void print(const std::vector<Stmt *>& statements);

private:
    void visitIncrementByte(IncrementByte *stmt) override;

    void visitDecrementByte(DecrementByte *stmt) override;

    void visitIncrementPtr(IncrementPtr *stmt) override;

    void visitDecrementPtr(DecrementPtr *stmt) override;

    void visitConditional(Conditional *stmt) override;

    void visitOutput(Output *stmt) override;

    void visitInput(Input *stmt) override;

    void visitDebug(Debug *stmt) override;

    void printLevel() const;
};

} // brainlove

#endif //ASTPRINTER_H
