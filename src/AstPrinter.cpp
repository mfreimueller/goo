//
// Created by michael on 10.06.25.
//

#include "AstPrinter.h"

#include <iostream>
#include <ostream>

namespace goo {
    void AstPrinter::print(const std::vector<Stmt *> &statements) {
        for (auto &stmt: statements) {
            if (stmt != nullptr) {
                stmt->accept(this);
            }
        }
    }

    void AstPrinter::visitIncrementByte(IncrementByte *stmt) {
        addIndentation();
        std::cout << "<IncrementByte> " << std::to_string(stmt->line) << ":" << std::to_string(stmt->column) <<
                std::endl;
    }

    void AstPrinter::visitDecrementByte(DecrementByte *stmt) {
        addIndentation();
        std::cout << "<DecrementByte> " << std::to_string(stmt->line) << ":" << std::to_string(stmt->column) <<
                std::endl;
    }

    void AstPrinter::visitIncrementPtr(IncrementPtr *stmt) {
        addIndentation();
        std::cout << "<IncrementPtr> " << std::to_string(stmt->line) << ":" << std::to_string(stmt->column) <<
                std::endl;
    }

    void AstPrinter::visitDecrementPtr(DecrementPtr *stmt) {
        addIndentation();
        std::cout << "<DecrementPtr> " << std::to_string(stmt->line) << ":" << std::to_string(stmt->column) <<
                std::endl;
    }

    void AstPrinter::visitInput(Input *stmt) {
        addIndentation();
        std::cout << "<Input> " << std::to_string(stmt->line) << ":" << std::to_string(stmt->column) << std::endl;
    }

    void AstPrinter::visitOutput(Output *stmt) {
        addIndentation();
        std::cout << "<Output> " << std::to_string(stmt->line) << ":" << std::to_string(stmt->column) << std::endl;
    }

    void AstPrinter::visitConditional(Conditional *stmt) {
        addIndentation();
        std::cout << "<Conditional> " << std::to_string(stmt->line) << ":" << std::to_string(stmt->column) << std::endl;

        depth++;
        print(stmt->stmts);
        depth--;
    }

    void AstPrinter::visitDebug(Debug *stmt) {
        addIndentation();
        std::cout << "<Debug> " << std::to_string(stmt->line) << ":" << std::to_string(stmt->column) << std::endl;
    }


    void AstPrinter::addIndentation() const {
        for (int i = 0; i < depth; i++) {
            std::cout << "\t";
        }
    }
} // goo
