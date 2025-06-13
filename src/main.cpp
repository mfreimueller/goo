#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <CLI/CLI.hpp>

#include "Scanner.h"
#include "Parser.h"
#include "CodeGen.h"
#include "Assembler.h"
#include "Util.h"
#include "Pipeline.h"

using namespace goo;
namespace fs = std::filesystem;

struct Config {
    bool debugBuild = false;
    bool emitAstTree = false;
    bool emitAsmCode = false;
    bool noOpt = false;
    bool verbose = false;

    std::string outputFile = "out.o";
};

int runFile(const std::string &filepath, const Config &config);

void runPrompt();

int main(const int argc, char **argv) {
    CLI::App app{
        "goo is a lightweight compiler for the programming language brainfuck, which create Linux-native ELF-binaries.",
        "goo"
    };

    Config config;

    app.add_flag("--emit-asm", config.emitAsmCode,
                 "Print the translated assembler code instead of producing an object file. This only produces an output if an input file is provided.");

    app.add_flag("-d,--debug", config.debugBuild,
                 "Enable the debug build, which includes debug information and symbols in the output file.");

    app.add_flag("--emit-ast", config.emitAstTree,
                 "Print the AST tree of the converted statements.");

    app.add_flag("--no-opt", config.noOpt,
                 "Disable any optimizations.");

    app.add_flag("-v,--verbose", config.verbose,
                 "Print more messages for easier debugging.");

    app.add_option("-o,--output", config.outputFile,
                   "Path to a file that contains either ELF code or assembler code.");

    app.allow_extras();
    CLI11_PARSE(app, argc, argv);

    if (const auto remainingArgs = app.remaining(); !remainingArgs.empty()) {
        runFile(remainingArgs[0], config);
    } else {
        runPrompt();
    }

    return 0;
}

/// Starts the REPL mode. This function repeatedly reads a users input, processes it
/// and prints results (if any).
///
/// To exit REPL mode, the user must either enter `exit` (case-insensitive) or press `Ctrl-D`.
void runPrompt() {
    Reporter reporter;
    std::string line;

    StandardPipelineBuilder builder(reporter);
    const auto pipeline = builder.stringInput()
            .lexer()
            .parser()
            .interpreter()
            .output()
            .build();

    while (true) {
        std::cout << "> ";

        try {
            getline(std::cin, line);
        } catch (const std::ifstream::failure &) {
            break;
        }

        if (const auto strippedLine = stripWhitespace(line); compareStringsCaseInsensitive(strippedLine, "exit")) {
            break;
        }

        auto payload = std::make_shared<StringPayload>(StringPayload{.value = line});
        // ReSharper disable once CppDFAUnusedValue
        auto _ = pipeline->execute(payload);

        reporter.reset();
    }
}

int runFile(const std::string &filepath, const Config &config) {
    Reporter reporter;
    StandardPipelineBuilder builder(reporter);

    const auto initialPayload = std::make_shared<FilePayload>(FilePayload{.filepath = filepath});

    builder.fileInput()
            .lexer()
            .parser();

    if (config.emitAstTree) {
        builder.astPrinter().output();
    } else {
        if (!config.noOpt) {
            builder.optimizer();
        }

        builder.codeGen(CodeGenConfig{
            .debugBuild = config.debugBuild
        });

        if (config.emitAsmCode) {
            builder.output();
        } else {
            builder.assembler(AssemblerConfig{
                .debugBuild = config.debugBuild,
                .verbose = config.verbose,
                .outputFile = config.outputFile
            });
        }
    }

    if (const auto pipeline = builder.build(); !pipeline->execute(initialPayload)) {
        return 1;
    }

    return 0;
}
