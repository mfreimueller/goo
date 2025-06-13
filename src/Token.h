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
        INC_BYTE, DEC_BYTE, INC_PTR, DEC_PTR, OUT, IN, IF, FI, DEBUG, EOF_, NONE
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
