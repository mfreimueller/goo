//
// Created by michael on 16.06.25.
//

#include "../src/Scanner.h"
#include "../src/Payload.h"
#include "../src/Reporter.h"

#include <catch2/catch_test_macros.hpp>

using namespace goo;

TEST_CASE("Scanner: + is being correctly transformed to INC_BYTE token", "[scanner]") {
    auto input = std::make_shared<StringPayload>(StringPayload { .value = "+" });

    Reporter reporter;
    Scanner scanner(reporter);
    auto result = std::static_pointer_cast<TokenPayload>(scanner.run(input));

    REQUIRE(result != nullptr);
    REQUIRE(result->tokens.size() == 2); // 2 because EOF_
    REQUIRE(result->tokens[0]->type == INC_BYTE);
}