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
```

## Usage

### Compilation of a brainfuck script

```bash
./goo -o hello.o hello.bf
```

This creates an ELF-compatible object-file `hello.o` that you can link using `ld`:

```bash
ld hello.o -o hello
./hello
```

### Print assembler code

```bash
./goo -s hello.bf
```

This returns the generated x86_64-NASM-code for the script `hello.bf`.

### Interactive REPL

```bash
./goo
```

This launches an interactive REPL for brainfuck. Enter `exit` or use `Ctrl+D` to exit.

## Project structure

```bash
.
├── CMakeLists.txt         # build definition
├── src/                   # source code for goo
│   ├── main.cpp
│   ├── compiler/
│   └── ...
└── examples/              # example scripts in brainfuck
```

## TODO

- Optimization of commands such as [->+<]
- Support for more platforms
- Improve REPL mode to support multiline inputs (for example, if a conditional is opened by not closed)

## License

This project is licensed under the MIT-license - see [LICENSE](LICENSE) for details.

## Contribution

Pull requests are welcome! For larger changes it'd be preferred to open an issues first.
