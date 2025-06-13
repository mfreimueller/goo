#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <CLI/CLI.hpp>

#include "Scanner.h"
#include "Parser.h"
#include "CodeGen.h"
#include "Interpreter.h"
#include "AsmBuilder.h"
#include "Optimizer.h"
#include "Util.h"
#include "Pipeline.h"

using namespace goo;
namespace fs = std::filesystem;

int runFile(const std::string &inputFile, const std::string &outputFile, bool printAsmCode, bool debugBuild);

void runPrompt();

std::vector<Stmt *> transform(const std::string &source, Reporter &reporter);

int compileAsmCode(std::string &code, const std::string &outputFile, bool debugBuild);

int main(const int argc, char **argv) {
    CLI::App app{
        "goo is a lightweight compiler for the programming language brainfuck, which create Linux-native ELF-binaries.",
        "goo"
    };

    bool printAsmCode = false;
    app.add_flag("-s", printAsmCode,
                 "Print the translated assembler code instead of producing an object file. This only produces an output if an input file is provided.");

    bool debugBuild = false;
    app.add_flag("-d", debugBuild,
                 "Enable the debug build, which includes debug information and symbols in the output file.");

    std::string outputFile = "out.o";
    app.add_option("-o,--output", outputFile, "Path to a file that contains either ELF code or assembler code.");

    app.allow_extras();
    CLI11_PARSE(app, argc, argv);

    Reporter reporter;
    StandardPipelineBuilder builder(reporter);

    if (const auto remainingArgs = app.remaining(); !remainingArgs.empty()) {
        const auto initialPayload = std::make_shared<FilePayload>(FilePayload{.filepath = remainingArgs.at(0)});

        builder.fileInput()
                .lexer()
                .parser() // TODO: add ASTPrinter here
                .optimizer() // TODO: make configurable
                .codeGen();

        if (printAsmCode) {
            builder.output();
        } else {
            builder.assembler();
        }

        if (const auto pipeline = builder.build(); !pipeline->execute(initialPayload)) {
            return 1;
        }
    } else {
        runPrompt();
    }

    // , outputFile, printAsmCode, debugBuild);

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
    auto pipeline = builder.stringInput()
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

        reporter.setCode(line);

        auto payload = std::make_shared<StringPayload>(StringPayload { .value = line });
        // ReSharper disable once CppDFAUnusedValue
        auto unused = pipeline->execute(payload);

        reporter.reset();

        std::cout << std::endl;
    }
}
