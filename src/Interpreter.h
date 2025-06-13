//
// Created by michael on 10.06.25.
//

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Payload.h"
#include "Pipeline.h"
#include "Stmt.h"

namespace goo {
    /// The core part of the REPL-mode. The interpreter walks the
    /// statement-tree and performs each statement, storing the data
    /// in an internal tape of type unsigned char.
    ///
    /// Because of the tape being of type unsigned char, when increment
    /// or decrement statements overflow or underflow, they remain in a
    /// brainfuck consistent state, being within 0 - 255. Negative numbers
    /// aren't possible, as 0-1 becomes 254 and vice versa.
    ///
    /// Both operations moving the tape pointer (> and <) also have a wrap-around
    /// guard implemented. Whenever the pointer would go below 0 or above 29,999
    /// they wrap around to their corresponding opposite, so 0-1 becomes 29,998
    /// and vice versa.
    ///
    /// A special debugging command has been added to the brainfuck command set,
    /// symbolized by the exclamation point (!). Using this command prints useful
    /// debug information about the current state of the Interpreter, such as the
    /// tape pointer and the tape itself.
    class Interpreter final : public Phase, public Visitor {
        unsigned char *tape;
        int tapePtr;

    public:
        explicit Interpreter(Reporter &reporter);
        ~Interpreter() override;

        /// Interprets a list of statements, modifying (if applicable) the internal type.
        /// This function returns no status code, but may set the warning or error flag.
        /// It is recommended to check both flags after execution to inform the user.
        /// If during execution an error occurs, the execution stops, leaving the tape possibly
        /// in an inconsistent state.
        /// @param payload A list of statements to interpret. Nullptr-entries are being ignored.
        std::shared_ptr<Payload> run(std::shared_ptr<Payload> payload) override;

    private:
        void interpret(const std::vector<std::shared_ptr<Stmt>> &stmts);

        void visitIncrementByte(IncrementByte *stmt) override;

        void visitDecrementByte(DecrementByte *stmt) override;

        void visitIncrementPtr(IncrementPtr *stmt) override;

        void visitDecrementPtr(DecrementPtr *stmt) override;

        void visitConditional(Conditional *stmt) override;

        void visitOutput(Output *stmt) override;

        void visitInput(Input *stmt) override;

        /// Prints the debugging information, containing the current line, column, the
        /// location of the tape pointer, as well as every entry in the internal tape
        /// that is currently not zero.
        void visitDebug(Debug *stmt) override;

        void visitReset(Reset *stmt) override;

        void visitTransfer(Transfer *stmt) override;
    };
} // goo

#endif //INTERPRETER_H
