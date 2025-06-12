#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <CLI/CLI.hpp>

#include "Scanner.h"
#include "Parser.h"
#include "Assembler.h"
#include "Interpreter.h"
#include "AsmBuilder.h"
#include "Util.h"

using namespace goo;
namespace fs = std::filesystem;

int runFile(const std::string &inputFile, const std::string &outputFile, bool printAsmCode);
void runPrompt();
std::vector<Stmt *> transform(const std::string& source);

int main(const int argc, char **argv) {
    CLI::App app{ "goo is a lightweight compiler for the programming language brainfuck, which create Linux-native ELF-binaries.", "goo" };

    bool printAsmCode = false;
    app.add_flag("-s", printAsmCode, "Print the translated assembler code instead of producing an object file. This only produces an output if an input file is provided.");

    std::string outputFile = "out.o";
    app.add_option("-o,--output", outputFile, "Path to a file that contains either ELF code or assembler code.");

    app.allow_extras();
    CLI11_PARSE(app, argc, argv);

    if (const auto remainingArgs = app.remaining(); !remainingArgs.empty()) {
        return runFile(remainingArgs.at(0), outputFile, printAsmCode);
    }

    runPrompt();
    return 0;
}

/// Reads a file, translates it to assembler code and either prints this code or
/// compiles it to create an ELF-object file.
///
/// @param inputFile The path to a file to read and translate/compile.
/// @param outputFile The path to a file to write the compiled ELF-object file to. This argument is ignored when `printAsmCode` is true.
/// @param printAsmCode True if the translated assembler code is to be written to standard out, otherwise the ELF-object file is created.
/// @return A status code according to [https://tldp.org/LDP/abs/html/exitcodes.html]
int runFile(const std::string& inputFile, const std::string &outputFile, bool printAsmCode) {
    auto ifs = std::ifstream(inputFile);
    const auto fileContent = std::string(std::istreambuf_iterator{ifs}, {});

    auto stmts = transform(fileContent);

    AsmBuilder *builder = new StringAsmBuilder;

    Assembler assembler(*builder);
    const auto asmCode = assembler.execute(stmts);

    for (const auto stmt : stmts) {
        delete stmt;
    }

    if (printAsmCode) {
        std::cout << asmCode << std::endl;
    } else {
        fs::path tmpDir = fs::temp_directory_path();
        fs::path tmpFile = tmpDir / fs::path("bf_tmpXXXXXX.asm");

        std::string tmpPath = tmpFile.string();
        int fd = mkstemps(tmpPath.data(), 4);
        if (fd == -1) {
            std::cerr << "Failed to create temporary file: " << tmpPath.data() << std::endl;
            return 1;
        }

        close(fd);

        std::ofstream out(tmpPath);
        out << asmCode;
        out.close();

        std::string cmd = "nasm -f elf64 " + tmpPath + " -o " + outputFile;
        if (int result = system(cmd.c_str()); result != 0) {
            std::cerr << "Failed to execute command: " << cmd << std::endl;
            return 1;
        }

        std::cout << "Created object file " << outputFile << std::endl;
        fs::remove(tmpFile);
    }

    return 0;
}

/// Starts the REPL mode. This function repeatedly reads a users input, processes it
/// and prints results (if any).
///
/// To exit REPL mode, the user must either enter `exit` (case-insensitive) or press `Ctrl-D`.
void runPrompt() {
    Interpreter interpreter;
    std::string line;

    while (true) {
        std::cout << "> ";

        try {
            getline(std::cin, line);
        } catch (const std::ifstream::failure &) {
            break;
        }

        line = stripWhitespace(line);
        if (compareStringsCaseInsensitive(line, "exit")) {
            break;
        }

        auto stmts = transform(line);
        interpreter.interpret(stmts);

        for (const auto stmt : stmts) {
            delete stmt;
        }

        std::cout << std::flush;
    }
}

/// Transforms brainfuck code into a list of statements which can be further processed.
///
/// @param source A string containing brainfuck code. This can either be a single command, a line or a whole script.
/// @return A list of translated statements which can be interpreted or compiled.
std::vector<Stmt *> transform(const std::string& source) {
    Scanner scanner(source);
    const auto tokens = scanner.scanTokens();

    Parser parser(tokens);
    return parser.parse();
}