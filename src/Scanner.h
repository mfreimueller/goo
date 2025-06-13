//
// Created by michael on 05.06.25.
//

#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>

#include "Payload.h"
#include "Pipeline.h"
#include "Token.h"

namespace goo {
    /// This class processes brainfuck source code and extracts tokens for each
    /// proper brainfuck statement. For debugging purposes, the scanner also tracks
    /// the current line and column within the line.
    ///
    /// The scanner doesn't execute or interpret any actual code, it merely turns
    /// each lexeme into a corresponding (dumb) token for further processing.
    class Scanner final : public Phase {
        std::string source;
        std::vector<std::shared_ptr<Token>> tokens;

        int current = 0;

        // track line and column to add this as debug info in case of compiler errors
        int line = 1;
        int column = 0;

    public:
        explicit Scanner(Reporter &reporter) : Phase(reporter) {
        }

        /// Scans the source and extracts the valid lexemes, turning them to tokens. This function
        /// doesn't check for syntax errors, instead it blindly turns the code into tokens.
        /// @return A list of tokens representing the valid brainfuck statements in the source provided.
        std::shared_ptr<Payload> run(std::shared_ptr<Payload> payload) override;

    private:
        /// Compares the value in `current` with the length of source to verify whether the end of
        /// source has been reached or not.
        /// @return True if the source has been processed completely, otherwise false.
        [[nodiscard]] bool isAtEnd() const;

        /// Scans the next character of source and matches it against a list of valid brainfuck statements. If
        /// a character is not a valid brainfuck statement, it is ignored. Otherwise, ::addToken is called with
        /// the proper type of the token.
        void scanToken();

        /// Adds a new token to the list of already processed tokens, based on the token type.
        /// The current line and column are added to the token for debugging purposes.
        /// @param type The type of the token to add.
        void addToken(TokenType type);
    };
} // goo

#endif //SCANNER_H
