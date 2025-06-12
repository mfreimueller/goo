//
// Created by michael on 05.06.25.
//

#ifndef TOKEN_H
#define TOKEN_H
#include <string>

namespace goo {

enum TokenType {
    INC_BYTE, DEC_BYTE, INC_PTR, DEC_PTR, OUT, IN, IF, FI, DEBUG, EOF_
};

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

    [[nodiscard]] std::string toString() const;
};

} // goo

#endif //TOKEN_H
