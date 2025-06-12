//
// Created by michael on 05.06.25.
//

#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>

#include "Token.h"

namespace goo {
    class Scanner {
        const std::string &source;
        std::vector<Token> tokens;

        int current = 0;

        // track line and column to add this as debug info in case of compiler errors
        int line = 1;
        int column = 0;

    public:
        explicit Scanner(const std::string &source) : source(source) {
        }

        std::vector<Token> scanTokens();

    private:
        [[nodiscard]] bool isAtEnd() const;

        void scanToken();

        void addToken(TokenType type);
    };
} // goo

#endif //SCANNER_H
