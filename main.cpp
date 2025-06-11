#include <fstream>
#include <iostream>
#include <iterator>

#include "Scanner.h"
#include "Parser.h"
#include "Assembler.h"
#include "AstPrinter.h"
#include "Interpreter.h"
#include "AsmBuilder.h"

using namespace brainlove;

int runFile(const std::string& fileName);
[[noreturn]] void runPrompt();
int run(const std::string& source);

int main(const int argc, char **argv) {
    if (argc > 2) {
        std::cout << "Usage: brainlove [script]" << std::endl;
        return 64;
    }

    if (argc == 2)
        return runFile(argv[1]);

    runPrompt();
    return 0;
}

int runFile(const std::string& fileName) {
    auto ifs = std::ifstream(fileName);
    const auto fileContent = std::string(std::istreambuf_iterator{ifs}, {});

    return run(fileContent);
}

[[noreturn]] void runPrompt() {
    std::string line;

    while (true) {
        std::cout << "> ";
        getline(std::cin, line); // TODO: how to check for errors?

        run(line);
    }
}

int run(const std::string& source) {
    Scanner scanner(source);
    const auto tokens = scanner.scanTokens();

    Parser parser(tokens);
    auto stmts = parser.parse();

    AstPrinter printer;
    // printer.print(stmts);

    Interpreter interpreter;
    //interpreter.interpret(stmts);

    AsmBuilder *builder = new StringAsmBuilder;

    Assembler assembler(*builder);
    const auto asmCode = assembler.execute(stmts);

    for (const auto stmt : stmts) {
        delete stmt;
    }
    stmts.clear();

    std::cout << asmCode << std::endl;

    return 0;
}