//
// Created by michael on 17.06.25.
//

#ifndef ASSEMBLEREMULATOR_H
#define ASSEMBLEREMULATOR_H
#include <map>
#include <sstream>
#include <string>
#include <vector>

struct Value {
    void *ptr;
    int value;
};

/// A basic assembler emulator that interprets assembler code. Used exclusively for testing to make sure that
/// optimized and unoptimized statements produce the same result, as well as making sure that the assembler code
/// is equivalent to the interpreted statements.
/// This emulator is not intended for production use, as its implementation is very crude.
class AssemblerEmulator {
    bool hasError = false;

    /// Points to the current line that is being executed.
    int execPtr = 0;

    std::vector<std::string> lines;

    /// A map containing a list of all labels as well as their line in the code.
    std::map<std::string, int> labels;

    int *tape;

    int cmpResult = 0;

    std::stringstream output;

    // registers
    int rax = 0, rbx = 0, rcx = 0, rdx = 0, rdi = 0, rsi = 0, r8 = 0, r9 = 0, r10 = 0;

public:
    AssemblerEmulator();

    ~AssemblerEmulator();

    std::string execute(const std::string &code);

private:
    int executeLine(int execPtr);

    // all commands supported by this emulator
    typedef const std::vector<std::string> &Params;

    int mov(Params args, int execPtr);

    int add(Params args, int execPtr);

    int cmp(Params args, int execPtr);

    int jge(Params args, int execPtr);

    int jle(Params args, int execPtr);

    int mul(Params args, int execPtr);

    int sub(Params args, int execPtr);

    int jmp(Params args, int execPtr);

    int syscall(Params args, int execPtr);

    void *getPointer(const std::string& label);

    Value getValue(const std::string& label);
};


#endif //ASSEMBLEREMULATOR_H
