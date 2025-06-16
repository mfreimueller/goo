add_library(goo_lib
        Scanner.cpp
        Scanner.h
        Token.cpp
        Token.h
        Parser.cpp
        Parser.h
        Stmt.cpp
        Stmt.h
        CodeGen.cpp
        CodeGen.h
        Interpreter.cpp
        Interpreter.h
        AstPrinter.cpp
        AstPrinter.h
        AsmBuilder.cpp
        AsmBuilder.h
        Util.cpp
        Util.h
        Reporter.cpp
        Reporter.h
        Optimizer.cpp
        Optimizer.h
        Pipeline.cpp
        Pipeline.h
        Input.cpp
        Input.h
        Payload.h
        Assembler.cpp
        Assembler.h
        Output.cpp
        Output.h
)

target_include_directories(goo_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(goo_lib PRIVATE CLI11::CLI11)

add_executable(goo main.cpp)
target_link_libraries(goo PRIVATE goo_lib)