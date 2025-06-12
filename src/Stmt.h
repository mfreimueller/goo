//
// Created by michael on 06.06.25.
//

#ifndef STMT_H
#define STMT_H
#include <string>
#include <vector>

namespace goo {
    class IncrementByte;
    class DecrementByte;
    class IncrementPtr;
    class DecrementPtr;
    class Conditional;
    class Output;
    class Input;
    class Debug;

    class Visitor;

    class Stmt {
    public:
        const int column;
        const int line;

        Stmt(const int column, const int line): column(column), line(line) {}
        virtual ~Stmt() = default;

        virtual void accept(Visitor *visitor) = 0;

        [[nodiscard]] std::string debugInfo() const {
            return std::to_string(line) + ":" + std::to_string(column);
        }
    };

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
    };

    class IncrementByte final : public Stmt {
    public:
        IncrementByte(const int column, const int line): Stmt(column, line) {}

        void accept(Visitor *visitor) override {
            visitor->visitIncrementByte(this);
        }
    };

    class DecrementByte final : public Stmt {
    public:
        DecrementByte(const int column, const int line): Stmt(column, line) {}

        void accept(Visitor *visitor) override {
            visitor->visitDecrementByte(this);
        }
    };

    class IncrementPtr final : public Stmt {
    public:
        IncrementPtr(const int column, const int line): Stmt(column, line) {}

        void accept(Visitor *visitor) override {
            visitor->visitIncrementPtr(this);
        }
    };

    class DecrementPtr final : public Stmt {
    public:
        DecrementPtr(const int column, const int line): Stmt(column, line) {}

        void accept(Visitor *visitor) override {
            visitor->visitDecrementPtr(this);
        }
    };

    class Conditional final : public Stmt {
    public:
        std::vector<Stmt *> stmts;

        explicit Conditional(const int column, const int line, const std::vector<Stmt *> &stmts): Stmt(column, line), stmts(stmts) {
        }

        ~Conditional() override;

        void accept(Visitor *visitor) override {
            visitor->visitConditional(this);
        }
    };

    class Output final : public Stmt {
    public:
        Output(const int column, const int line): Stmt(column, line) {}

        void accept(Visitor *visitor) override {
            visitor->visitOutput(this);
        }
    };

    class Input final : public Stmt {
    public:
        Input(const int column, const int line): Stmt(column, line) {}

        void accept(Visitor *visitor) override {
            visitor->visitInput(this);
        }
    };

    class Debug final : public Stmt {
    public:
        Debug(const int column, const int line): Stmt(column, line) {}

        void accept(Visitor *visitor) override {
            visitor->visitDebug(this);
        }
    };
} // goo

#endif //STMT_H
