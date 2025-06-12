//
// Created by michael on 06.06.25.
//

#ifndef PARSER_H
#define PARSER_H
#include <utility>
#include <vector>
#include "Stmt.h"

namespace brainlove {
    class Token;

    class Parser {
        std::vector<Token> tokens;

        int current = 0;

    public:
        explicit Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {};

        std::vector<Stmt *> parse();

    private:
        Stmt *statement();

        Conditional *conditional(int column, const int line);

        [[nodiscard]] bool isAtEnd() const;
        [[nodiscard]] Token peek() const;
    };
} // brainlove

#endif //PARSER_H
