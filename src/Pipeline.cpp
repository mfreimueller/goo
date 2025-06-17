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

    //
    // DebugPhase
    //

    std::shared_ptr<Payload> DebugPhase::run(std::shared_ptr<Payload> payload) {
        switch (type) {
            case FILE:
                value = std::static_pointer_cast<FilePayload>(payload)->filepath;
                break;
            case STRING:
                value = std::static_pointer_cast<StringPayload>(payload)->value;
                break;
            case TOKEN:
                tokens = std::static_pointer_cast<TokenPayload>(payload)->tokens;
                break;
            case STMT:
                stmts = std::static_pointer_cast<StmtPayload>(payload)->stmts;
                break;
            default:
                break;
        }

        return payload;
    }

    //
    // StandardPipelineBuilder
    //

    StandardPipelineBuilder::StandardPipelineBuilder(Reporter &reporter): _reporter(reporter) {
    }

    std::unique_ptr<Pipeline> StandardPipelineBuilder::build() {
        return std::make_unique<Pipeline>(phases, _reporter);
    }

    PipelineBuilder &StandardPipelineBuilder::fileInput() {
        phases.emplace_back(std::make_shared<FileInput>(_reporter));
        return *this;
    }

    PipelineBuilder &StandardPipelineBuilder::stringInput() {
        phases.emplace_back(std::make_shared<StringInput>(_reporter));
        return *this;
    }

    PipelineBuilder &StandardPipelineBuilder::lexer() {
        phases.emplace_back(std::make_shared<Scanner>(_reporter));
        return *this;
    }

    PipelineBuilder &StandardPipelineBuilder::parser() {
        phases.emplace_back(std::make_shared<Parser>(_reporter));
        return *this;
    }

    PipelineBuilder &StandardPipelineBuilder::interpreter() {
        return interpreter(std::cout);
    }

    PipelineBuilder &StandardPipelineBuilder::interpreter(std::ostream &out) {
        phases.emplace_back(std::make_shared<Interpreter>(_reporter, out));
        return *this;
    }

    PipelineBuilder &StandardPipelineBuilder::codeGen(CodeGenConfig config) {
        auto asmBuilder = std::static_pointer_cast<AsmBuilder>(std::make_shared<StringAsmBuilder>());
        phases.emplace_back(std::make_shared<CodeGen>(config, asmBuilder, _reporter));
        return *this;
    }

    PipelineBuilder &StandardPipelineBuilder::optimizer() {
        phases.emplace_back(std::make_shared<Optimizer>(_reporter));
        return *this;
    }

    PipelineBuilder &StandardPipelineBuilder::assembler(AssemblerConfig config) {
        phases.emplace_back(std::make_shared<Assembler>(config, _reporter));
        return *this;
    }

    PipelineBuilder &StandardPipelineBuilder::astPrinter() {
        phases.emplace_back(std::make_shared<AstPrinter>(_reporter));
        return *this;
    }

    PipelineBuilder &StandardPipelineBuilder::output() {
        phases.emplace_back(std::make_shared<OutputPhase>(_reporter));
        return *this;
    }

    PipelineBuilder &StandardPipelineBuilder::debug(std::shared_ptr<DebugPhase> debugPhase) {
        phases.emplace_back(debugPhase);
        return *this;
    }

} // goo
