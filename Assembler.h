//
// Created by michael on 10.06.25.
//

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include "Stmt.h"
#include "AsmBuilder.h"

namespace brainlove {
    class Assembler final : public Visitor {
        int labelCounter = 0;

        AsmBuilder &builder;

    public:
        explicit Assembler(AsmBuilder &_builder) : builder(_builder) {};
        ~Assembler() override = default;

        std::string execute(const std::vector<Stmt*>& statements);

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
}

#endif //ASSEMBLER_H
