//
// Created by michael on 16.06.25.
//

#include <catch2/catch_test_macros.hpp>

#include "../src/Optimizer.h"
#include "../src/Parser.h"
#include "../src/Pipeline.h"

using namespace goo;

/*
 * This test case is more of an integration test, because for reasons of
 * convenience we use a Scanner-Parser pipeline to parse a brainfuck code
 * into a list of statements.
 */

void recursivePatternMatch(const StmtVector &stmts, const std::vector<std::pair<TokenType, int> > &expected, int &idx);

std::shared_ptr<Payload> mockStmts(const std::string &code) {
    Reporter reporter;

    const auto debugPhase = std::make_shared<DebugPhase>(STMT, reporter);

    StandardPipelineBuilder builder(reporter);
    const auto &pipeline = builder.stringInput()
            .lexer()
            .parser()
            .debug(debugPhase)
            .build();

    bool success = pipeline->execute(std::make_shared<StringPayload>(StringPayload{.value = code}));
    REQUIRE(success);

    return std::make_shared<StmtPayload>(StmtPayload { .stmts = debugPhase->getStmts() });
}

void checkGroupings(const std::string &inputCode, const std::vector<std::pair<TokenType, int> > &expected) {
    Reporter reporter;
    Optimizer optimizer(reporter);
    auto result = optimizer.run(mockStmts(inputCode));
    REQUIRE(result != nullptr);

    const auto stmtPayload = std::static_pointer_cast<StmtPayload>(result);

    int idx = 0;
    recursivePatternMatch(stmtPayload->stmts, expected, idx);
    REQUIRE(idx == expected.size());
}

void recursivePatternMatch(const StmtVector &stmts, const std::vector<std::pair<TokenType, int> > &expected, int &expectedIdx) {
    for (int idx = 0, expIdx = expectedIdx; idx < stmts.size() && idx < expected.size(); idx++, expIdx = expectedIdx) {
        REQUIRE(stmts[idx]->type == expected[expIdx].first);

        int count = -1;
        if (stmts[idx]->type == INC_BYTE) {
            count = std::static_pointer_cast<IncrementByte>(stmts[0])->count;
        } else if (stmts[idx]->type == DEC_BYTE) {
            count = std::static_pointer_cast<DecrementByte>(stmts[0])->count;
        } else if (stmts[idx]->type == INC_PTR) {
            count = std::static_pointer_cast<IncrementPtr>(stmts[0])->count;
        } else if (stmts[idx]->type == DEC_PTR) {
            count = std::static_pointer_cast<DecrementPtr>(stmts[0])->count;
        } else if (stmts[idx]->type == IF) {
            const auto &conditional = std::static_pointer_cast<Conditional>(stmts[idx]);

            if (!conditional->stmts.empty()) {
                recursivePatternMatch(conditional->stmts, expected, ++expectedIdx);
                expectedIdx--;
            }
            count = 1;
        }

        if (count != -1) {
            REQUIRE(count == expected[expIdx].second);
        }

        expectedIdx++;
    }
}

TEST_CASE("Optimizer: make sure that statements are correctly grouped", "[optimizer]") {
    checkGroupings("++", {{INC_BYTE, 2}});
    checkGroupings("---", {{DEC_BYTE, 3}});
    checkGroupings(">>", {{INC_PTR, 2}});
    checkGroupings("<<<", {{DEC_PTR, 3}});
}

TEST_CASE("Optimizer: make sure that non-groupable statements are not grouped", "[optimizer]") {
    checkGroupings("+,+", {{INC_BYTE, 1}, {IN, 1}, {INC_BYTE, 1}});
    checkGroupings("-<-", {{DEC_BYTE, 1}, {DEC_PTR, 1}, {DEC_BYTE, 1}});
    checkGroupings(">+>", {{INC_PTR, 1}, {INC_BYTE, 1}, {INC_PTR, 1}});
    checkGroupings("<-<", {{DEC_PTR, 1}, {DEC_BYTE, 1}, {DEC_PTR, 1}});
    checkGroupings(",,", {{IN, 1}, {IN, 1}});
    checkGroupings("..", {{OUT, 1}, {OUT, 1}});
    checkGroupings("[[]]", {});
}

TEST_CASE("Optimizer: make sure that conditionals are correctly grouped", "[optimizer]") {
    checkGroupings("[++]", {{IF, 1}, {INC_BYTE, 2}});
    checkGroupings("[[>>]]", {{IF, 1}, {IF, 1}, {INC_PTR, 2}});
}
