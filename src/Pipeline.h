//
// Created by michael on 13.06.25.
//

#ifndef PIPELINE_H
#define PIPELINE_H
#include <memory>
#include <utility>
#include <vector>

#include "Payload.h"

namespace goo {
    struct CodeGenConfig;
}

namespace goo {
    struct AssemblerConfig;

    class Phase;
    class PipelineBuilder;
    class Reporter;

    /// A pipeline combines different phases of compilation and manages the
    /// execution of each phase, by retrieving the payload of the currently processing
    /// unit and passing it over to the next unit.
    ///
    /// The core idea is to have a list of pluggable compiler phases that can be
    /// switched on or off based on user flags.
    ///
    /// To create a new pipeline, use the PipelineBuilder.
    class Pipeline {
        Reporter &reporter;
        std::vector<std::shared_ptr<Phase> > phases;

    public:
        explicit Pipeline(std::vector<std::shared_ptr<Phase> > phases, Reporter &reporter) : reporter(reporter),
            phases(std::move(phases)) {
        }

        ~Pipeline();

        /// Executes the pipeline by sequentially calling each phase and handing over payloads. If at any point an
        /// error occurs, the pipeline is stopped, all resources are released and false is returned.
        /// @param initialPayload The payload to use for the first phase of the compiler, typically StringPayload or
        /// @return True if the execution was successful (i.e. without errors), otherwise false.
        [[nodiscard]] bool execute(const std::shared_ptr<Payload> &initialPayload) const;
    };

    /// A compiler phase which is part of a pipeline. A phase can be anything from extracting code from a source file
    /// to processing statements and converting them to ASM. Note that instances of Phase and all of its inheriting
    /// classes are exclusively to be constructed by the PipelineBuilder to unify the process.
    class Phase {
    protected:
        Reporter &reporter;

    public:
        virtual ~Phase() = default;

        explicit Phase(Reporter &reporter): reporter(reporter) {
        }

        /// Runs the phase, processing the received payload and transforming it into a new payload that is being returned
        /// to the caller.
        /// @param payload A payload to process. If the payload is not of the expected type, the behavior of the method is undefined.
        /// @return Returns a new payload that contains the processed datastream.
        virtual std::shared_ptr<Payload> run(std::shared_ptr<Payload> payload) = 0;
    };

    /// The payload type that the debug phase is about to retrieve.
    /// This is important for correct casting and copying.
    enum PayloadType {
        FILE,
        STRING,
        TOKEN,
        STMT
    };

    /// A special Phase that is used for unit testing to interject a pipeline
    /// and retrieve the current payload.
    class DebugPhase final : public Phase {
        PayloadType type;

        std::vector<std::shared_ptr<Stmt>> stmts;
        std::vector<std::shared_ptr<Token>> tokens;
        std::string value;

    public:
        explicit DebugPhase(const PayloadType type, Reporter &reporter) : Phase(reporter), type(type) {
        }

        [[nodiscard]] std::vector<std::shared_ptr<Stmt>> getStmts() const { return stmts; }
        [[nodiscard]] std::vector<std::shared_ptr<Token>> getTokens() const { return tokens; }
        [[nodiscard]] std::string getValue() const { return value; }

        std::shared_ptr<Payload> run(std::shared_ptr<Payload> payload) override;
    };

    /// A utility class that constructs a pipeline based on a specific list of phases. Note that the builder is
    /// expected to be called in a proper order, for example the first phase ought to be either ::fileInput or
    /// ::stringInput. Calling ::lexer first results in undefined behavior.
    class PipelineBuilder {
    public:
        virtual ~PipelineBuilder() = default;

        [[nodiscard]] virtual std::unique_ptr<Pipeline> build() = 0;

        virtual PipelineBuilder &fileInput() = 0;

        virtual PipelineBuilder &stringInput() = 0;

        virtual PipelineBuilder &lexer() = 0;

        virtual PipelineBuilder &parser() = 0;

        virtual PipelineBuilder &optimizer() = 0;

        virtual PipelineBuilder &interpreter() = 0;

        virtual PipelineBuilder &codeGen(CodeGenConfig config) = 0;

        virtual PipelineBuilder &assembler(AssemblerConfig config) = 0;

        virtual PipelineBuilder &astPrinter() = 0;

        virtual PipelineBuilder &output() = 0;

        virtual PipelineBuilder &debug(std::shared_ptr<DebugPhase> debugPhase) = 0;
    };

    /// A default implementation of PipelineBuilder that constructs a pipeline based on a list of phases.
    class StandardPipelineBuilder final : public PipelineBuilder {
        Reporter &_reporter;
        std::vector<std::shared_ptr<Phase> > phases;

    public:
        explicit StandardPipelineBuilder(Reporter &reporter);

        [[nodiscard]] std::unique_ptr<Pipeline> build() override;

        PipelineBuilder &fileInput() override;

        PipelineBuilder &stringInput() override;

        PipelineBuilder &lexer() override;

        PipelineBuilder &parser() override;

        PipelineBuilder &optimizer() override;

        PipelineBuilder &interpreter() override;

        PipelineBuilder &codeGen(CodeGenConfig config) override;

        PipelineBuilder &assembler(AssemblerConfig config) override;

        PipelineBuilder &astPrinter() override;

        PipelineBuilder &output() override;

        PipelineBuilder &debug(std::shared_ptr<DebugPhase> debugPhase) override;
    };
} // goo

#endif //PIPELINE_H
