//
// Created by michael on 10.06.25.
//

#ifndef ASTPRINTER_H
#define ASTPRINTER_H
#include "Stmt.h"

namespace goo {
    /// A debugging visitor that prints the abstract syntax tree to
    /// check for errors in transforming tokens to statements.
    class AstPrinter final : public Visitor {
        /// The current depth of indentation, used primarily for
        /// conditionals, which gets indented recursively.
        int depth = 0;

    public:
        /// Traverses a list of statements and prints the generated
        /// AST to standard out. Nullptr-entries are being ignored.
        void print(const std::vector<Stmt *> &statements);

    private:
        void visitIncrementByte(IncrementByte *stmt) override;

        void visitDecrementByte(DecrementByte *stmt) override;

        void visitIncrementPtr(IncrementPtr *stmt) override;

        void visitDecrementPtr(DecrementPtr *stmt) override;

        void visitConditional(Conditional *stmt) override;

        void visitOutput(Output *stmt) override;

        void visitInput(Input *stmt) override;

        void visitDebug(Debug *stmt) override;

        /// Adds as many \t as the current value of depth holds to standard out.
        /// This method is called by every ::visitXYZ method and has thusly been moved to a separate function.
        void addIndentation() const;
    };
} // goo

#endif //ASTPRINTER_H
