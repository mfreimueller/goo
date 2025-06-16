//
// Created by michael on 05.06.25.
//

#ifndef TOKEN_H
#define TOKEN_H
#include <string>

namespace goo {
    /// An enumeration of all possible *valid* token types
    /// that are processed by goo. Although some tokens, such as FI
    /// or EOF_ are tracked, they aren't processed any further (currently).
    enum TokenType {
        INC_BYTE = 1,
        DEC_BYTE = 2,
        INC_PTR = 4,
        DEC_PTR = 8,
        OUT = 16,
        IN = 32,
        IF = 64,
        FI = 128,
        DEBUG = 256,
        EOF_ = 512,
        NONE = 1024
    };

    /// A token is a textual representation of a valid brainfuck statement.
    /// Tokens can be used to analyze the code and perform further optimization
    /// steps. They are not intended to be interpreted or translated directly.
    /// Instead, tokens are transformed into statements.
    class Token {
    public:
        const int line;
        const int column;

        const TokenType type;

        Token(const TokenType type, const int line, const int column)
            : line(line),
              column(column),
              type(type) {
        }

        /// Returns a textual representation of this token, mostly for debugging purposes.
        /// @return A string of this format: "<lexeme> <line>:<column>"
        [[nodiscard]] std::string toString() const;
    };
} // goo

#endif //TOKEN_H
