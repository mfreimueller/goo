//
// Created by michael on 06.06.25.
//

#ifndef PARSER_H
#define PARSER_H
#include <utility>
#include <vector>
#include "Stmt.h"

namespace goo {
    class Token;

    /// Parses a list of tokens, turning them into statements, which
    /// can be processed. The parser performs syntax-checking (as far as
    /// brainfuck contains possibilities for malformed syntax).
    /// As each parser is initialized with one list of tokens, to process
    /// a new list of tokens (for example, in REPL mode), it is necessary
    /// to create a new instance.
    class Parser {
        std::vector<Token> tokens;

        int current = 0;

    public:
        explicit Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {};

        /// Parses the list of tokens provided in the constructor. Each token is transformed into a
        /// statement. In case of syntax errors the error flag is set and a new error message is being
        /// added, although the parser still tries to parse the following tokens. It is not recommended
        /// to interpret or compile the statements in case of error.
        /// @return A list of statements. The caller of this function must take care to release the memory.
        std::vector<Stmt *> parse();

    private:
        /// Transforms the next token into a statement, performing basic syntax error checks. In case of error,
        /// the error flag is set and nullptr is returned.
        /// @return The statement represented by the current token. Nullptr in case of error.
        Stmt *statement();

        /// Creates a new conditional-statement, recursively calling ::statement() for each child-statement, as
        /// long as the corresponding FI-statement is not reached. Due to the recursive nature of this function,
        /// child-conditionals can be properly processed.
        /// @param column The column of the current token.
        /// @param line The line of the current line.
        /// @return The conditional-statement represented by the current token.
        Conditional *conditional(int column, int line);

        [[nodiscard]] bool isAtEnd() const;
        [[nodiscard]] Token peek() const;
    };
} // goo

#endif //PARSER_H
