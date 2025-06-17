//
// Created by michael on 16.06.25.
//

#include <catch2/catch_test_macros.hpp>

#include "../src/CodeGen.h"
#include "../src/Optimizer.h"
#include "../src/Parser.h"
#include "../src/Pipeline.h"
#include "AssemblerEmulator.h"

using namespace goo;

/*
 * These test cases use the AssemblerEmulator to compare the results of
 * optimized and unoptimized statements.
 */

std::unique_ptr<Pipeline> unoptimizedPipeline(std::shared_ptr<DebugPhase> &debugPhase, Reporter &reporter) {
    StandardPipelineBuilder builder(reporter);
    auto ptr = builder.stringInput()
            .lexer()
            .parser()
            .codeGen(CodeGenConfig{.debugBuild = false})
            .debug(debugPhase)
            .build();
    return ptr;
}

std::unique_ptr<Pipeline> optimizedPipeline(std::shared_ptr<DebugPhase> &debugPhase, Reporter &reporter) {
    StandardPipelineBuilder builder(reporter);
    return builder.stringInput()
            .lexer()
            .parser()
            .optimizer()
            .codeGen(CodeGenConfig{.debugBuild = false})
            .debug(debugPhase)
            .build();
}

void testStmts(const std::string &code) {
    Reporter reporter;
    auto debugPhase = std::make_shared<DebugPhase>(STRING, reporter);

    auto unoptimized = unoptimizedPipeline(debugPhase, reporter);
    auto optimized = optimizedPipeline(debugPhase, reporter);

    const auto &payload = std::make_shared<StringPayload>(StringPayload{.value = code});

    REQUIRE(unoptimized->execute(payload));
    const auto &unoptimizedCode = debugPhase->getValue();

    REQUIRE(optimized->execute(payload));
    const auto &optimizedCode = debugPhase->getValue();

    auto assembler = new AssemblerEmulator;
    const auto &unoptimizedResult = assembler->execute(unoptimizedCode);
    delete assembler;

    assembler = new AssemblerEmulator;
    const auto &optimizedResult = assembler->execute(optimizedCode);
    delete assembler;

    REQUIRE(unoptimizedResult == optimizedResult);
}

TEST_CASE("Optimizer: make sure that multiply statements are optimized correctly", "[optimizer]") {
    testStmts("++[>++++<-]>.");
    testStmts("+[->+<]>.");
    testStmts(">+[-<+>]<.");
}

TEST_CASE("Optimizer: make sure that reset statements are optimized correctly", "[optimizer]") {
    testStmts("++++.[-].");
}

TEST_CASE("Optimizer: make sure that grouping statements are optimized correctly", "[optimizer]") {
    testStmts("++++.");
    testStmts("----.");
    testStmts("++--++.");
    testStmts("--++--.");
    testStmts("+-+-+-.");
    testStmts("+>>>+.");
    testStmts("[+-+-].");
}

TEST_CASE("Optimizer: make sure that statements are not grouped when mixed", "[optimizer]") {
    testStmts("+>+<+>+.");
    testStmts("-<->-<-.");
    testStmts("+>->+>+.");
}
