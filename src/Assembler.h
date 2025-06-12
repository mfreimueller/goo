//
// Created by michael on 10.06.25.
//

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include "Stmt.h"
#include "AsmBuilder.h"

namespace goo {
    /**
     * A visitor to statements that produces assembler code
     */
    class Assembler final : public Visitor {
        int labelCounter = 0;

        AsmBuilder &builder;

    public:
        explicit Assembler(AsmBuilder &_builder) : builder(_builder) {};
        ~Assembler() override = default;

        /**
         * Translates the provided statements into assembler code.
         *
         * Note, that this function must not be called recursively, as it appends the terminate statement
         * at the end of the function call. In order to process statements recursively, use a for-loop
         * and call ::accept(this).
         *
         * @param statements A list of statements that are to be executed (i.e. translated into assembler code).
         * @return A string containing the complete assembler code for the given input statements.
         */
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
