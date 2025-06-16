//
// Created by michael on 10.06.25.
//

#ifndef ASTPRINTER_H
#define ASTPRINTER_H

#include <string>
#include "Pipeline.h"
#include "Stmt.h"

namespace goo {
    /// A debugging visitor that prints the abstract syntax tree to
    /// check for errors in transforming tokens to statements.
    class AstPrinter final : public Phase, public Visitor {
        /// The current depth of indentation, used primarily for
        /// conditionals, which gets indented recursively.
        int depth = 0;
        std::string output;

    public:
        explicit AstPrinter(Reporter &reporter) : Phase(reporter) {}

        /// Traverses a list of statements and prints the generated
        /// AST to standard out. Nullptr-entries are being ignored.
        std::shared_ptr<Payload> run(std::shared_ptr<Payload> payload) override;

    private:
        void print(const std::vector<std::shared_ptr<Stmt>>& stmts);

        void visitIncrementByte(IncrementByte *stmt) override;

        void visitDecrementByte(DecrementByte *stmt) override;

        void visitIncrementPtr(IncrementPtr *stmt) override;

        void visitDecrementPtr(DecrementPtr *stmt) override;

        void visitConditional(Conditional *stmt) override;

        void visitOutput(Output *stmt) override;

        void visitInput(Input *stmt) override;

        void visitDebug(Debug *stmt) override;

        void visitReset(Reset *stmt) override;

        void visitTransfer(Transfer *stmt) override;

        void visitMultiply(Multiply *stmt) override;

        /// Adds as many \t as the current value of depth holds to standard out.
        /// This method is called by every ::visitXYZ method and has thusly been moved to a separate function.
        std::string indentation() const;
    };
} // goo

#endif //ASTPRINTER_H
