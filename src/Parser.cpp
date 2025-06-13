//
// Created by michael on 06.06.25.
//

#include "Parser.h"

#include <memory>

#include "Payload.h"
#include "Token.h"

namespace goo {

    std::shared_ptr<Payload> Parser::run(const std::shared_ptr<Payload> payload) {
        // reset state of parser to allow for reuse
        current = 0;

        const auto tokenPayload = std::static_pointer_cast<TokenPayload>(payload);
        tokens = tokenPayload->tokens;

        std::vector<std::shared_ptr<Stmt>> stmts;
        while (!isAtEnd()) {
            stmts.emplace_back(std::shared_ptr<Stmt>(statement()));
        }

        return std::make_shared<StmtPayload>(StmtPayload { .stmts = stmts});
    }

    Stmt *Parser::statement() { // NOLINT(*-no-recursion)
        switch (const auto token = tokens.at(current++); token->type) {
            case INC_BYTE:
                return new IncrementByte(token->column, token->line);
            case DEC_BYTE:
                return new DecrementByte(token->column, token->line);
            case INC_PTR:
                return new IncrementPtr(token->column, token->line);
            case DEC_PTR:
                return new DecrementPtr(token->column, token->line);
            case OUT:
                return new Output(token->column, token->line);
            case IN:
                return new Input(token->column, token->line);
            case IF:
                return conditional(token->column, token->line);
            case FI:
                // If we reach this case, we have a closing tag ] without any
                // opening. This is treated as a syntax error.
                reporter.error(token->line, token->column, "Unexpected closing tag ]");
                break;
            case DEBUG:
                return new Debug(token->column, token->line);
            default:
                // At this point there should be no unknown token type. Having
                // reached this point means that we encountered an error.
                reporter.error(token->line, token->column, "Unexpected tag");
                break;
        }

        return nullptr;
    }

    Conditional *Parser::conditional(const int column, const int line) { // NOLINT(*-no-recursion)
        std::vector<std::shared_ptr<Stmt>> stmts;

        while (peek()->type != FI && !isAtEnd()) {
            stmts.emplace_back(statement());
        }

        // skip the FI token
        current++;

        return new Conditional(column, line, stmts);
    }

    bool Parser::isAtEnd() const {
        return peek()->type == EOF_;
    }

    std::shared_ptr<Token> Parser::peek() const {
        return this->tokens[current];
    }

} // goo