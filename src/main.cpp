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

int runFile(const std::string &inputFile, const std::string &outputFile, bool printAsmCode, bool debugBuild);
void runPrompt();
std::vector<Stmt *> transform(const std::string& source, Reporter &reporter);
int compileAsmCode(std::string &code, const std::string &outputFile, bool debugBuild);

int main(const int argc, char **argv) {
    CLI::App app{ "goo is a lightweight compiler for the programming language brainfuck, which create Linux-native ELF-binaries.", "goo" };

    bool printAsmCode = false;
    app.add_flag("-s", printAsmCode, "Print the translated assembler code instead of producing an object file. This only produces an output if an input file is provided.");

    bool debugBuild = false;
    app.add_flag("-d", debugBuild, "Enable the debug build, which includes debug information and symbols in the output file.");

    std::string outputFile = "out.o";
    app.add_option("-o,--output", outputFile, "Path to a file that contains either ELF code or assembler code.");

    app.allow_extras();
    CLI11_PARSE(app, argc, argv);

    if (const auto remainingArgs = app.remaining(); !remainingArgs.empty()) {
        return runFile(remainingArgs.at(0), outputFile, printAsmCode, debugBuild);
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

    auto reporter = Reporter(inputFile);
    reporter.setCode(fileContent);

    auto stmts = transform(fileContent, reporter);

    std::string asmCode;
    if (!reporter.hasError()) {
        AsmBuilder *builder = new StringAsmBuilder;

        Assembler assembler(*builder);
        asmCode = assembler.execute(stmts);
    }

    for (const auto stmt : stmts) {
        delete stmt;
    }

    reporter.print();

    // Cancel further execution in case of errors, while ignoring warnings.
    if (reporter.hasError()) {
        return 1;
    }

    if (printAsmCode) {
        std::cout << asmCode << std::endl;
    } else {
        return compileAsmCode(asmCode, outputFile, debugBuild);
    }

    return 0;
}

/// Starts the REPL mode. This function repeatedly reads a users input, processes it
/// and prints results (if any).
///
/// To exit REPL mode, the user must either enter `exit` (case-insensitive) or press `Ctrl-D`.
void runPrompt() {
    Reporter reporter;
    auto interpreter = Interpreter(reporter);
    std::string line;

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

        auto stmts = transform(line, reporter);

        // We treat errors as show-stoppers and only interpret lines if no errors occurred.
        // Warnings we report afterward, if any occurred.
        if (!reporter.hasError()) {
            interpreter.interpret(stmts);
        }

        reporter.print();
        reporter.reset();

        for (const auto stmt : stmts) {
            delete stmt;
        }

        std::cout << std::endl;
    }
}

/// Transforms brainfuck code into a list of statements which can be further processed.
///
/// @param source A string containing brainfuck code. This can either be a single command, a line or a whole script.
/// @param reporter A reporter that tracks any syntactical errors and warnings, to be reported to the user afterward.
/// @return A list of translated statements which can be interpreted or compiled.
std::vector<Stmt *> transform(const std::string& source, Reporter &reporter) {
    Scanner scanner(source);
    const auto tokens = scanner.scanTokens();

    Parser parser(tokens, reporter);
    return parser.parse();
}

/// Compiles assembler code into an ELF-object file, by first writing the code into a temporary file and then
/// invoking `nasm` to compile the file. Note, that this function doesn't verify that `nasm` is actually installed
/// on the users computer, as this is listed in the requirements in the README file.
/// @param code The assembler code to compile.
/// @param outputFile The path of the output file containing the ELF-object file.
/// @param debugBuild True if debug symbols should be included in the output file.
/// @return A status code corresponding to [https://tldp.org/LDP/abs/html/exitcodes.html].
int compileAsmCode(std::string &code, const std::string &outputFile, bool debugBuild) {
    fs::path tmpDir = fs::temp_directory_path();

    // We need to add the XXXXXX to the filepath, because mkstemps replaces this with random characters.
    fs::path tmpFile = tmpDir / fs::path("bf_tmpXXXXXX.asm");

    std::string tmpPath = tmpFile.string();
    int fd = mkstemps(tmpPath.data(), 4);
    if (fd == -1) {
        std::cerr << "Failed to create temporary file: " << tmpPath.data() << std::endl;
        return 1;
    }

    close(fd);

    std::ofstream out(tmpPath);
    out << code;
    out.close();

    std::string cmd;

    if (debugBuild) {
        cmd = "nasm -f elf64 -g -F dwarf " + tmpPath + " -o " + outputFile;
    } else {
        cmd = "nasm -f elf64 " + tmpPath + " -o " + outputFile;
    }

    std::cout << cmd << std::endl;

    if (int result = system(cmd.c_str()); result != 0) {
        std::cerr << "Failed to execute command: " << cmd << std::endl;
        return 1;
    }

    std::cout << "Created object file " << outputFile << std::endl;
    fs::remove(tmpFile);

    return 0;
}