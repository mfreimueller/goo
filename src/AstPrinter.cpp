//
// Created by michael on 10.06.25.
//

#include "AstPrinter.h"

#include <iostream>
#include <ostream>

namespace brainlove {
    void AstPrinter::print(const std::vector<Stmt *>& statements) {
        for (auto& stmt : statements) {
            stmt->accept(this);
        }
    }

    void AstPrinter::visitIncrementByte(IncrementByte *stmt) {
        printLevel();
        std::cout << "<IncrementByte> " << std::to_string(stmt->line) << ":" << std::to_string(stmt->column) << std::endl;
    }

    void AstPrinter::visitDecrementByte(DecrementByte *stmt) {
        printLevel();
        std::cout << "<DecrementByte> " << std::to_string(stmt->line) << ":" << std::to_string(stmt->column) << std::endl;
    }

    void AstPrinter::visitIncrementPtr(IncrementPtr *stmt) {
        printLevel();
        std::cout << "<IncrementPtr> " << std::to_string(stmt->line) << ":" << std::to_string(stmt->column) << std::endl;
    }

    void AstPrinter::visitDecrementPtr(DecrementPtr *stmt) {
        printLevel();
        std::cout << "<DecrementPtr> " << std::to_string(stmt->line) << ":" << std::to_string(stmt->column) << std::endl;
    }

    void AstPrinter::visitInput(Input *stmt) {
        printLevel();
        std::cout << "<Input> " << std::to_string(stmt->line) << ":" << std::to_string(stmt->column) << std::endl;
    }

    void AstPrinter::visitOutput(Output *stmt) {
        printLevel();
        std::cout << "<Output> " << std::to_string(stmt->line) << ":" << std::to_string(stmt->column) << std::endl;
    }

    void AstPrinter::visitConditional(Conditional *stmt) {
        printLevel();
        std::cout << "<Conditional> " << std::to_string(stmt->line) << ":" << std::to_string(stmt->column) << std::endl;

        level++;
        print(stmt->stmts);
        level--;
    }

    void AstPrinter::visitDebug(Debug *stmt) {
        printLevel();
        std::cout << "<Debug> " << std::to_string(stmt->line) << ":" << std::to_string(stmt->column) << std::endl;
    }


    void AstPrinter::printLevel() const {
        for (int i = 0; i < level; i++) {
            std::cout << "\t";
        }
    }

} // brainlove