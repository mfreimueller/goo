//
// Created by michael on 10.06.25.
//

#ifndef CODEGEN_H
#define CODEGEN_H

#include <utility>

#include "Stmt.h"
#include "AsmBuilder.h"
#include "Pipeline.h"

namespace goo {
    struct CodeGenConfig {
        const bool debugBuild;
    };

    /// The core feature of goo, CodeGen traverses a list of statements and produces corresponding assembler code.
    /// An internal counter is maintained for keeping track of any label, to implement unique labels for guards
    /// and loops.
    ///
    /// Note that it is unsafe to call ::execute more than once, as CodeGen doesn't reset the AsmBuilder instance,
    /// thusly keeping the created code, as well as keeping the internal label counter.
    class CodeGen final : public Phase, public Visitor {
        int labelCounter = 0;

        const CodeGenConfig config;
        std::shared_ptr<AsmBuilder> builder;

    public:
        explicit CodeGen(const CodeGenConfig config, std::shared_ptr<AsmBuilder> builder, Reporter &reporter) : Phase(reporter), config(config), builder(std::move(builder)) {};
        ~CodeGen() override = default;

        /// Translates the provided statements into assembler code.
        /// Note, that this function must not be called recursively, as it appends the terminate statement
        /// at the end of the function call. In order to process statements recursively, use a for-loop
        /// and call ::accept(this).
        ///
        /// Additionally, after having processed all statements, an exit syscall is added to the code. Therefore,
        /// although it is possible to call execute more than once, it wouldn't have any effect, as the code
        /// would terminate after having processed the first generated code.
        ///
        /// @param payload A payload of type StmtPayload.
        /// @return A payload of type StringPayload containing the complete assembler code for the given input statements.
        std::shared_ptr<Payload> run(std::shared_ptr<Payload> payload) override;

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
