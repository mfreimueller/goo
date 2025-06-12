cmake_minimum_required(VERSION 3.31)
project(goo)

set(CMAKE_CXX_STANDARD 20)

find_package(CLI11 CONFIG REQUIRED)

include_directories(src)

add_executable(goo src/main.cpp
        src/Scanner.cpp
        src/Scanner.h
        src/Token.cpp
        src/Token.h
        src/Parser.cpp
        src/Parser.h
        src/Stmt.cpp
        src/Stmt.h
        src/Assembler.cpp
        src/Assembler.h
        src/Interpreter.cpp
        src/Interpreter.h
        src/AstPrinter.cpp
        src/AstPrinter.h
        src/AsmBuilder.cpp
        src/AsmBuilder.h
        src/Util.cpp
        src/Util.h
        src/Reporter.cpp
        src/Reporter.h
        src/Optimizer.cpp
        src/Optimizer.h)

target_link_libraries(goo PRIVATE CLI11::CLI11)