//
// Created by michael on 16.06.25.
//

#include "../src/Scanner.h"
#include "../src/Payload.h"
#include "../src/Reporter.h"

#include <catch2/catch_test_macros.hpp>

using namespace goo;

void checkTokenSequence(const std::string &input, const std::vector<TokenType> &expected) {
    auto payload = std::make_shared<StringPayload>(StringPayload { .value = input });
    Reporter reporter;
    Scanner scanner(reporter);
    auto result = std::static_pointer_cast<TokenPayload>(scanner.run(payload));

    REQUIRE(result != nullptr);
    REQUIRE(result->tokens.size() == expected.size() + 1); // +1 because EOF_

    for (int idx = 0; idx < result->tokens.size() - 1; ++idx) { // -1 because EOF_
        REQUIRE(result->tokens[idx]->type == expected[idx]);
    }
}

TEST_CASE("Scanner: make sure that unary statements are detected correctly", "[scanner]") {
    checkTokenSequence("+", { INC_BYTE });
    checkTokenSequence("-", { DEC_BYTE });
    checkTokenSequence(">", { INC_PTR });
    checkTokenSequence("<", { DEC_PTR });
    checkTokenSequence(",", { IN });
    checkTokenSequence(".", { OUT });
    checkTokenSequence("[", { IF });
    checkTokenSequence("]", { FI });
}

TEST_CASE("Scanner: make sure that multiple statements are detected correctly", "[scanner]") {
    checkTokenSequence("++", { INC_BYTE, INC_BYTE });
    checkTokenSequence("->+<-", { DEC_BYTE, INC_PTR, INC_BYTE, DEC_PTR, DEC_BYTE });
    checkTokenSequence(">[+]", { INC_PTR, IF, INC_BYTE, FI });
    checkTokenSequence(",.", { IN, OUT});
}

TEST_CASE("Scanner: make sure that any non-statements are ignored", "[scanner]") {
    checkTokenSequence("ab", { });
    checkTokenSequence("a+b", { INC_BYTE });
    checkTokenSequence("{]", { FI });
}