# goo — A Brainfuck Compiler for Linux (ELF)

`goo` is a lightweight compiler for the programming language [brainfuck](https://en.wikipedia.org/wiki/Brainfuck), which create Linux-native ELF-binaries.
`goo` either translates brainfuck-code directly to Assembler or creates an object file. Additionally, `goo` provides an interactive REPL mode.

---

## Features

- Compiles brainfuck to **ELF64 object-files**
- Optionally returns **x86_64 NASM assembler code**
- Provides a **REPL-mode**
- No runtime dependencies - created binaries are completely independent

---

## Build Guide

### Requirements

- CMake (Version 3.12 or above)
- C++17-compatible compiler (z.B. `g++` oder `clang++`)
- [`nasm`](https://www.nasm.us/) (for assembling the created ASM-files)
- optionally: [`ld`](https://man7.org/linux/man-pages/man1/ld.1.html) to link the object files

### Build with CMake

```bash
git clone https://github.com/mfreimueller/goo.git
cd goo
mkdir build && cd build
cmake ..
make

## Roadmap

- Optimizations: Although the costs of execution of the
    commands produced by a brainf*ck program should be minimal,
    for educational purposes I intend to introduce some optimizations,
    for example grouping of statements, such as ADD or SUB.
- Remove redundant checks 