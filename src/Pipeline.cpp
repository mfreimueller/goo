//
// Created by michael on 13.06.25.
//

#include "Pipeline.h"

#include <memory>

#include "AsmBuilder.h"
#include "Assembler.h"
#include "AstPrinter.h"
#include "CodeGen.h"
#include "Input.h"
#include "Interpreter.h"
#include "Optimizer.h"
#include "Output.h"
#include "Parser.h"
#include "Reporter.h"
#include "Scanner.h"

namespace goo {
    Pipeline::~Pipeline() {
        phases.clear();
    }

    [[nodiscard]] bool Pipeline::execute(const std::shared_ptr<Payload> &initialPayload) const {
        if (initialPayload == nullptr) {
            reporter.error("Internal Error: Pipeline::execute invoked without any initial payload.");
            return false;
        }

        auto payload = initialPayload;

        for (const auto &phase: phases) {
            payload = phase->run(payload);

            if (reporter.hasWarnings() || reporter.hasError()) {
                reporter.print();

                if (reporter.hasError()) {
                    return false;
                }
            }
        }

        return true;
    }

    Phase::Phase(Reporter &reporter): reporter(reporter) {
    }

    StandardPipelineBuilder::StandardPipelineBuilder(Reporter &reporter): _reporter(reporter) {
    }

    std::unique_ptr<Pipeline> StandardPipelineBuilder::build() {
        return std::make_unique<Pipeline>(phases, _reporter);
    }

    PipelineBuilder &StandardPipelineBuilder::fileInput() {
        phases.emplace_back(std::make_unique<FileInput>(_reporter));
        return *this;
    }

    PipelineBuilder &StandardPipelineBuilder::stringInput() {
        phases.emplace_back(std::make_unique<StringInput>(_reporter));
        return *this;
    }

    PipelineBuilder &StandardPipelineBuilder::lexer() {
        phases.emplace_back(std::make_unique<Scanner>(_reporter));
        return *this;
    }

    PipelineBuilder &StandardPipelineBuilder::parser() {
        phases.emplace_back(std::make_unique<Parser>(_reporter));
        return *this;
    }

    PipelineBuilder &StandardPipelineBuilder::interpreter() {
        phases.emplace_back(std::make_unique<Interpreter>(_reporter));
        return *this;
    }

    PipelineBuilder &StandardPipelineBuilder::codeGen(CodeGenConfig config) {
        auto asmBuilder = std::static_pointer_cast<AsmBuilder>(std::make_shared<StringAsmBuilder>());
        phases.emplace_back(std::make_unique<CodeGen>(config, asmBuilder, _reporter));
        return *this;
    }

    PipelineBuilder &StandardPipelineBuilder::optimizer() {
        phases.emplace_back(std::make_unique<Optimizer>(_reporter));
        return *this;
    }

    PipelineBuilder &StandardPipelineBuilder::assembler(AssemblerConfig config) {
        phases.emplace_back(std::make_unique<Assembler>(config, _reporter));
        return *this;
    }

    PipelineBuilder &StandardPipelineBuilder::astPrinter() {
        phases.emplace_back(std::make_unique<AstPrinter>(_reporter));
        return *this;
    }

    PipelineBuilder &StandardPipelineBuilder::output() {
        phases.emplace_back(std::make_unique<OutputPhase>(_reporter));
        return *this;
    }
} // goo
