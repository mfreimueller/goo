//
// Created by michael on 16.06.25.
//

#include <catch2/catch_test_macros.hpp>

#include "../src/Parser.h"

using namespace goo;

std::vector<std::shared_ptr<Stmt>> parse(const std::vector<TokenType> &input) {
    std::vector<std::shared_ptr<Token>> tokens;
    for (const auto &type : input) {
        tokens.emplace_back(new Token(type, 0, 0));
    }

    const auto payload = std::make_shared<TokenPayload>(TokenPayload { .tokens = tokens });

    Reporter reporter;
    Parser parser(reporter);
    auto result = std::static_pointer_cast<StmtPayload>(parser.run(payload));

    REQUIRE(result != nullptr);

    return result->stmts;
}

void checkStatementSequence(const std::vector<TokenType> &input, const std::vector<TokenType> &expected) {
    const auto result = parse(input);

    REQUIRE(result.size() == expected.size());

    for (int idx = 0; idx < result.size(); idx++) {
        REQUIRE(result[idx]->type == expected[idx]);
    }
}

TEST_CASE("Parser: make sure that unary statements are parsed correctly", "[parser]") {
    checkStatementSequence({ INC_BYTE, EOF_ }, { INC_BYTE });
    checkStatementSequence({ DEC_BYTE, EOF_ }, { DEC_BYTE });
    checkStatementSequence({ INC_PTR, EOF_ }, { INC_PTR });
    checkStatementSequence({ DEC_PTR, EOF_ }, { DEC_PTR });
    checkStatementSequence({ IN, EOF_ }, { IN });
    checkStatementSequence({ OUT, EOF_ }, { OUT });
    checkStatementSequence({ IF, EOF_ }, { IF });
}

TEST_CASE("Parser: make sure that wild ] statements are treated as errors", "[parser]") {
    std::vector<std::shared_ptr<Token>> tokens;
    tokens.emplace_back(new Token(FI, 0, 0));
    tokens.emplace_back(new Token(EOF_, 0, 0));

    const auto payload = std::make_shared<TokenPayload>(TokenPayload { .tokens = tokens });

    Reporter reporter;
    Parser parser(reporter);
    auto result = std::static_pointer_cast<StmtPayload>(parser.run(payload));

    REQUIRE(result != nullptr);
    REQUIRE(reporter.hasError());
}

TEST_CASE("Parser: make sure that unknown statements are treated as errors", "[parser]") {
    std::vector<std::shared_ptr<Token>> tokens;
    tokens.emplace_back(new Token(NONE, 0, 0));
    tokens.emplace_back(new Token(EOF_, 0, 0));

    const auto payload = std::make_shared<TokenPayload>(TokenPayload { .tokens = tokens });

    Reporter reporter;
    Parser parser(reporter);
    auto result = std::static_pointer_cast<StmtPayload>(parser.run(payload));

    REQUIRE(result != nullptr);
    REQUIRE(reporter.hasError());
}

TEST_CASE("Parser: make sure that conditionals are being folded correctly", "[parser]") {
    checkStatementSequence({ IF, INC_BYTE, FI, EOF_ }, { IF });
}

TEST_CASE("Parser: make sure that deeply nested conditionals are being parsed correctly", "[parser]") {
    const auto result = parse({ IF, IF, IF, IF, IF, INC_BYTE, FI, FI, FI, FI, FI, EOF_ });
    int expectedDepth = 5;

    REQUIRE(result[0]->type == IF);

    auto current = result[0];
    while (current->type == IF) {
        expectedDepth--;

        const auto conditional = std::static_pointer_cast<Conditional>(current);
        REQUIRE(conditional->stmts.size() == 1);
        current = conditional->stmts[0];
    }

    REQUIRE(current->type == INC_BYTE);
    REQUIRE(expectedDepth == 0);
}

