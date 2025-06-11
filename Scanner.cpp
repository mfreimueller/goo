//
// Created by michael on 05.06.25.
//

#include "Scanner.h"

namespace brainlove {
    std::vector<Token> Scanner::scanTokens() {
        while (!isAtEnd()) {
            scanToken();
        }

        addToken(EOF_);
        return tokens;
    }

    bool Scanner::isAtEnd() const {
        return current >= source.size();
    }

    void Scanner::scanToken() {
        column++;

        switch (source[current++]) {
            case '+': addToken(INC_BYTE);
                break;
            case '-': addToken(DEC_BYTE);
                break;
            case '>': addToken(INC_PTR);
                break;
            case '<': addToken(DEC_PTR);
                break;
            case '.': addToken(OUT);
                break;
            case ',': addToken(IN);
                break;
            case '[': addToken(IF);
                break;
            case ']': addToken(FI);
                break;
            case '!': addToken(DEBUG);
                break;
            case '\n':
                line++;
                column = 0;
                break;
            default:
                // whitespace and any other character are treated as comments
                break;
        }
    }

    void Scanner::addToken(TokenType type) {
        tokens.emplace_back(type, line, column);
    }
} // brainlove
