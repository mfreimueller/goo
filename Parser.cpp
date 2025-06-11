//
// Created by michael on 06.06.25.
//

#include "Parser.h"

#include "Token.h"

namespace brainlove {

    std::vector<Stmt *> Parser::parse() {
        std::vector<Stmt *> stmts;
        while (!isAtEnd()) {
            stmts.emplace_back(statement());
        }

        return stmts;
    }

    Stmt *Parser::statement() {
        auto token = tokens.at(current++);

        switch (token.type) {
            case INC_BYTE:
                return new IncrementByte(token.column, token.line);
            case DEC_BYTE:
                return new DecrementByte(token.column, token.line);
            case INC_PTR:
                return new IncrementPtr(token.column, token.line);
            case DEC_PTR:
                return new DecrementPtr(token.column, token.line);
            case OUT:
                return new Output(token.column, token.line);
            case IN:
                return new Input(token.column, token.line);
            case IF:
                return conditional(token.column, token.line);
            case FI:
                // TODO: throw error?
                break;
            case DEBUG:
                return new Debug(token.column, token.line);
                break;
            default:
                break;
        }

        return nullptr;
    }

    Conditional *Parser::conditional(const int column, const int line) {
        std::vector<Stmt *> stmts;

        while (peek().type != FI && !isAtEnd()) {
            stmts.emplace_back(statement());
        }

        // skip the FI token
        current++;

        return new Conditional(column, line, stmts);
    }


    bool Parser::isAtEnd() const {
        return peek().type == EOF_;
    }

    Token Parser::peek() const {
        return this->tokens[current];
    }

} // brainlove