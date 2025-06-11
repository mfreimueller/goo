cmake_minimum_required(VERSION 3.31)
project(brainlove)

set(CMAKE_CXX_STANDARD 20)

add_executable(brainlove main.cpp
        Scanner.cpp
        Scanner.h
        Token.cpp
        Token.h
        Parser.cpp
        Parser.h
        Stmt.cpp
        Stmt.h
        Assembler.cpp
        Assembler.h
        Interpreter.cpp
        Interpreter.h
        AstPrinter.cpp
        AstPrinter.h
        AsmBuilder.cpp
        AsmBuilder.h)
