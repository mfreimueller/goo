//
// Created by michael on 16.06.25.
//

#include <iostream>
#include <catch2/catch_test_macros.hpp>

#include "../src/CodeGen.h"
#include "../src/Optimizer.h"
#include "../src/Parser.h"
#include "../src/Pipeline.h"
#include "AssemblerEmulator.h"

using namespace goo;

/*
 * These test cases use the AssemblerEmulator and the Interpreter to make sure that
 * the results are identical with both.
 */

std::unique_ptr<Pipeline> interpreterPipeline(Reporter &reporter, std::ostream &out) {
    StandardPipelineBuilder builder(reporter);
    auto ptr = builder.stringInput()
            .lexer()
            .parser()
            .interpreter(out)
            .build();
    return ptr;
}

std::unique_ptr<Pipeline> assemblerPipeline(const std::shared_ptr<DebugPhase> &debugPhase, Reporter &reporter) {
    StandardPipelineBuilder builder(reporter);
    return builder.stringInput()
            .lexer()
            .parser()
            .optimizer()
            .codeGen(CodeGenConfig{.debugBuild = false})
            .debug(debugPhase)
            .build();
}

void testAssemblerInterpreterStmts(const std::string &code) {
    Reporter reporter;
    auto debugPhase = std::make_shared<DebugPhase>(STRING, reporter);
    std::stringstream buffer;

    auto interpreter = interpreterPipeline(reporter, buffer);
    auto assembler = assemblerPipeline(debugPhase, reporter);

    const auto &payload = std::make_shared<StringPayload>(StringPayload{.value = code});

    REQUIRE(interpreter->execute(payload));

    const auto &interpreterResult = buffer.str();

    REQUIRE(assembler->execute(payload));
    const auto &assemblerCode = debugPhase->getValue();

    AssemblerEmulator asmEmu;
    const auto &assemblerResult = asmEmu.execute(assemblerCode);

    REQUIRE(interpreterResult == assemblerResult);
}

TEST_CASE("Interpreter-Optimizer: make sure that simple statements are processed correctly", "[interpreter-optimizer]") {
    testAssemblerInterpreterStmts("+.");
    testAssemblerInterpreterStmts("++-.");
    testAssemblerInterpreterStmts(">+<.");
}

TEST_CASE("Interpreter-Optimizer: make sure that multiply statements are processed correctly", "[interpreter-optimizer]") {
    testAssemblerInterpreterStmts("++[>++++<-]>.");
    testAssemblerInterpreterStmts("+[->+<]>.");
    testAssemblerInterpreterStmts(">+[-<+>]<.");
}

TEST_CASE("Interpreter-Optimizer: make sure that reset statements are processed correctly", "[interpreter-optimizer]") {
    testAssemblerInterpreterStmts("++++.[-].");
}

TEST_CASE("Interpreter-Optimizer: make sure that grouping statements are processed correctly", "[interpreter-optimizer]") {
    testAssemblerInterpreterStmts("++++.");
    testAssemblerInterpreterStmts("---.");
    testAssemblerInterpreterStmts("++--++.");
    testAssemblerInterpreterStmts("--++--.");
    testAssemblerInterpreterStmts("+-+-+-.");
    testAssemblerInterpreterStmts("+>>>+.");
    testAssemblerInterpreterStmts("[+-+-].");
}

TEST_CASE("Interpreter-Optimizer: make sure that statements are not grouped when mixed", "[interpreter-optimizer]") {
    testAssemblerInterpreterStmts("+>+<+>+.");
    testAssemblerInterpreterStmts("-<->-<-.");
    testAssemblerInterpreterStmts("+>->+>+.");
}
