//
// Created by michael on 10.06.25.
//

#include "AstPrinter.h"

#include <format>
#include <iostream>
#include <ostream>

namespace goo {
    std::shared_ptr<Payload> AstPrinter::run(const std::shared_ptr<Payload> payload) {
        // reset state for further reuse
        depth = 0;
        output = "";

        const auto stmtPayload = std::static_pointer_cast<StmtPayload>(payload);
        print(stmtPayload->stmts);

        return std::make_shared<StringPayload>(StringPayload{.value = output});
    }

    void AstPrinter::print(const std::vector<std::shared_ptr<Stmt> > &stmts) {
        for (const auto &stmt: stmts) {
            if (stmt != nullptr) {
                stmt->accept(this);
            }
        }
    }

    void AstPrinter::visitIncrementByte(IncrementByte *stmt) {
        output += std::format("{}<IncrementByte> {}:{}\n", indentation(), stmt->line, stmt->column);
    }

    void AstPrinter::visitDecrementByte(DecrementByte *stmt) {
        output += std::format("{}<DecrementByte> {}:{}\n", indentation(), stmt->line, stmt->column);
    }

    void AstPrinter::visitIncrementPtr(IncrementPtr *stmt) {
        output += std::format("{}<IncrementPtr> {}:{}\n", indentation(), stmt->line, stmt->column);
    }

    void AstPrinter::visitDecrementPtr(DecrementPtr *stmt) {
        output += std::format("{}<DecrementPtr> {}:{}\n", indentation(), stmt->line, stmt->column);
    }

    void AstPrinter::visitInput(Input *stmt) {
        output += std::format("{}<Input> {}:{}\n", indentation(), stmt->line, stmt->column);
    }

    void AstPrinter::visitOutput(Output *stmt) {
        output += std::format("{}<Output> {}:{}\n", indentation(), stmt->line, stmt->column);
    }

    void AstPrinter::visitConditional(Conditional *stmt) {
        output += std::format("{}<Conditional> {}:{}\n", indentation(), stmt->line, stmt->column);

        depth++;
        print(stmt->stmts);
        depth--;
    }

    void AstPrinter::visitDebug(Debug *stmt) {
        output += std::format("{}<Debug> {}:{}\n", indentation(), stmt->line, stmt->column);
    }

    void AstPrinter::visitReset(Reset *stmt) {
        output += std::format("{}<Reset> {}:{}\n", indentation(), stmt->line, stmt->column);
    }

    void AstPrinter::visitTransfer(Transfer *stmt) {
        output += std::format("{}<Transfer> {}:{}\n", indentation(), stmt->line, stmt->column);
    }

    void AstPrinter::visitMultiply(Multiply *stmt) {
        output += std::format("{}<Multiply> {}:{}\n", indentation(), stmt->line, stmt->column);
    }


    std::string AstPrinter::indentation() const {
        std::string _indentation;
        for (int i = 0; i < depth; i++) {
            _indentation += "\t";
        }

        return _indentation;
    }
} // goo
