//
// Created by michael on 10.06.25.
//

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include "Stmt.h"
#include "AsmBuilder.h"

namespace goo {
    /// The core feature of goo, Assembler traverses a list of statements and produces corresponding assembler code.
    /// An internal counter is maintained for keeping track of any label, to implement unique labels for guards
    /// and loops.
    ///
    /// Note that it is unsafe to call ::execute more than once, as Assembler doesn't reset the AsmBuilder instance,
    /// thusly keeping the created code, as well as keeping the internal label counter.
    class Assembler final : public Visitor {
        int labelCounter = 0;

        AsmBuilder &builder;

    public:
        explicit Assembler(AsmBuilder &_builder) : builder(_builder) {};
        ~Assembler() override = default;

        /// Translates the provided statements into assembler code.
        /// Note, that this function must not be called recursively, as it appends the terminate statement
        /// at the end of the function call. In order to process statements recursively, use a for-loop
        /// and call ::accept(this).
        ///
        /// Additionally, after having processed all statements, an exit syscall is added to the code. Therefore,
        /// although it is possible to call execute more than once, it wouldn't have any effect, as the code
        /// would terminate after having processed the first generated code.
        ///
        /// @param statements A list of statements that are to be translated into assembler code.
        /// @return A string containing the complete assembler code for the given input statements.
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
