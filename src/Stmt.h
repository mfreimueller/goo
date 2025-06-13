//
// Created by michael on 06.06.25.
//

#ifndef STMT_H
#define STMT_H
#include <memory>
#include <string>
#include <vector>

#include "Token.h"

namespace goo {
    class IncrementByte;
    class DecrementByte;
    class IncrementPtr;
    class DecrementPtr;
    class Conditional;
    class Output;
    class Input;
    class Debug;
    class Reset;
    class Transfer;

    class Visitor;

    /// A statement is a single command that performs a certain operation. This class
    /// represents an abstract base for any actual brainfuck statements.
    ///
    /// Statements are used by the Interpreter or the CodeGen to execute the brainfuck
    /// code or translate it to assembler. A statement itself is 'dumb', in that it doesn't
    /// know how to process itself. Instead, each processing unit, such as the Interpreter,
    /// implements the Visitor-interface, which calls ::accept(), which in turn
    /// calls the Visitor.
    ///
    /// For debugging purposes, each statement tracks its line and column, to make it
    /// easier to pinpoint errors.
    class Stmt {
    public:
        const int column;
        const int line;
        const TokenType type;

        Stmt(const int column, const int line, const TokenType type): column(column), line(line), type(type) {
        }

        virtual ~Stmt() = default;

        /// Attempts to match the pattern provided as param. For regular statements, only one token type must be
        /// provided as pattern, otherwise this call fails. For conditionals, the pattern includes all sub-statements.
        ///
        /// @param pattern A pattern to match.
        /// @return True if the statement matches the exact pattern, otherwise false.
        [[nodiscard]] virtual bool matches(std::vector<TokenType> pattern) const;

        /// Guides a visitor to the correct function to process this statement. Each
        /// inheriting class of Stmt must call the proper function.
        /// @param visitor A visitor that processes this statement.
        virtual void accept(Visitor *visitor) = 0;

        [[nodiscard]] std::string debugInfo() const {
            return std::to_string(line) + ":" + std::to_string(column);
        }
    };

    /// A visitor to a statement. The purpose of the visitor is to process a
    /// statement, as statements themselves are 'dumb' and contain no business logic.
    /// For each statement type there must be a corresponding ::visitXYZ function
    /// which processes this particular statement.
    class Visitor {
    public:
        virtual ~Visitor() = default;

        virtual void visitIncrementByte(IncrementByte *stmt) = 0;

        virtual void visitDecrementByte(DecrementByte *stmt) = 0;

        virtual void visitIncrementPtr(IncrementPtr *stmt) = 0;

        virtual void visitDecrementPtr(DecrementPtr *stmt) = 0;

        virtual void visitConditional(Conditional *stmt) = 0;

        virtual void visitOutput(Output *stmt) = 0;

        virtual void visitInput(Input *stmt) = 0;

        virtual void visitDebug(Debug *stmt) = 0;

        virtual void visitReset(Reset *stmt) = 0;

        virtual void visitTransfer(Transfer *stmt) = 0;
    };

    class IncrementByte final : public Stmt {
    public:
        const int count;

        IncrementByte(const int column, const int line,
                      const int count = 1): Stmt(column, line, INC_BYTE), count(count) {
        }

        void accept(Visitor *visitor) override {
            visitor->visitIncrementByte(this);
        }
    };

    class DecrementByte final : public Stmt {
    public:
        const int count;

        DecrementByte(const int column, const int line,
                      const int count = 1): Stmt(column, line, DEC_BYTE), count(count) {
        }

        void accept(Visitor *visitor) override {
            visitor->visitDecrementByte(this);
        }
    };

    class IncrementPtr final : public Stmt {
    public:
        const int count;

        IncrementPtr(const int column, const int line, const int count = 1): Stmt(column, line, INC_PTR),
                                                                             count(count) {
        }

        void accept(Visitor *visitor) override {
            visitor->visitIncrementPtr(this);
        }
    };

    class DecrementPtr final : public Stmt {
    public:
        const int count;

        DecrementPtr(const int column, const int line, const int count = 1): Stmt(column, line, DEC_PTR),
                                                                             count(count) {
        }

        void accept(Visitor *visitor) override {
            visitor->visitDecrementPtr(this);
        }
    };

    class Conditional final : public Stmt {
    public:
        std::vector<std::shared_ptr<Stmt> > stmts;

        explicit Conditional(const int column, const int line,
                             const std::vector<std::shared_ptr<Stmt> > &stmts): Stmt(column, line, IF), stmts(stmts) {
        }

        /// Matches the pattern against this conditional. Only the complete conditional can be matches, therefor
        /// the pattern must begin with IF and end with FI, otherwise the behavior is unpredictable.
        /// @param pattern A pattern to match. It must begin with IF and end with FI.
        /// @return True if the pattern matches the conditional.
        bool matches(std::vector<TokenType> pattern) const;

        void accept(Visitor *visitor) override {
            visitor->visitConditional(this);
        }
    };

    class Output final : public Stmt {
    public:
        Output(const int column, const int line): Stmt(column, line, OUT) {
        }

        void accept(Visitor *visitor) override {
            visitor->visitOutput(this);
        }
    };

    class Input final : public Stmt {
    public:
        Input(const int column, const int line): Stmt(column, line, IN) {
        }

        void accept(Visitor *visitor) override {
            visitor->visitInput(this);
        }
    };

    class Debug final : public Stmt {
    public:
        Debug(const int column, const int line): Stmt(column, line, DEBUG) {
        }

        void accept(Visitor *visitor) override {
            visitor->visitDebug(this);
        }
    };

    /// A reset statement is an optimized version of a [-] statement which resets a byte back to 0. The reset statement
    /// therefor performs this operation with one assignment, instead of a loop. The reset can take place at the
    /// location of the tape pointer (tapePtrOffset = 0), or relative to it (tapePtrOffset != 0).
    class Reset final : public Stmt {
    public:
        const int initialValue;
        const int tapePtrOffset;

        Reset(const int column, const int line, const int initialValue, const int tapePtrOffset): Stmt(column, line,
                NONE), initialValue(initialValue), tapePtrOffset(tapePtrOffset) {
        }

        void accept(Visitor *visitor) override {
            visitor->visitReset(this);
        }
    };

    /// A statement that transfers a value from one byte of the tape to another. The transfer can be a simple 1:1 one
    /// ( [>+<-] ) or more complex, such as [>++<-] or [>+<--] etc. The add/subOffset inform processing compiler phases
    /// about where to write the bytes to and where to read them from.
    class Transfer final : public Stmt {
    public:
        const int addOffset;
        const int subOffset;

        Transfer(const int column, const int line, const int addOffset, const int subOffset): Stmt(column, line,
                NONE), addOffset(addOffset), subOffset(subOffset) {
        }

        void accept(Visitor *visitor) override {
            visitor->visitTransfer(this);
        }
    };
} // goo

#endif //STMT_H
