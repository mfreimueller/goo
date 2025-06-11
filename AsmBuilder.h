//
// Created by michael on 11.06.25.
//

#ifndef ASMBUILDER_H
#define ASMBUILDER_H
#include <string>

namespace brainlove {
    class AsmBuilder {
    public:
        virtual ~AsmBuilder() = default;

        virtual std::string build() const = 0;

        virtual AsmBuilder &mov(std::string dest, std::string src) = 0;

        virtual AsmBuilder &label(std::string name) = 0;

        virtual AsmBuilder &x_or(std::string dest, std::string src) = 0;

        virtual AsmBuilder &syscall() = 0;

        virtual AsmBuilder &add(std::string dest, std::string src) = 0;

        virtual AsmBuilder &sub(std::string dest, std::string src) = 0;

        virtual AsmBuilder &lea(std::string dest, std::string src) = 0;

        virtual AsmBuilder &cmp(std::string dest, std::string src) = 0;

        virtual AsmBuilder &jmp(std::string label) = 0;

        virtual AsmBuilder &jg(std::string label) = 0;

        virtual AsmBuilder &jle(std::string label) = 0;

        virtual AsmBuilder &jge(std::string label) = 0;

        virtual AsmBuilder &comment(std::string comment) = 0;

        virtual AsmBuilder &newLine() = 0;
    };

    class StringAsmBuilder final : public AsmBuilder {
        std::string code;
    public:
        StringAsmBuilder();

        [[nodiscard]] std::string build() const override;

        AsmBuilder &mov(std::string dest, std::string src) override;

        AsmBuilder &label(std::string name) override;

        AsmBuilder &x_or(std::string dest, std::string src) override;

        AsmBuilder &syscall() override;

        AsmBuilder &add(std::string dest, std::string src) override;

        AsmBuilder &sub(std::string dest, std::string src) override;

        AsmBuilder &lea(std::string dest, std::string src) override;

        AsmBuilder &cmp(std::string dest, std::string src) override;

        AsmBuilder &jmp(std::string label) override;

        AsmBuilder &jg(std::string label) override;

        AsmBuilder &jle(std::string label) override;

        AsmBuilder &jge(std::string label) override;

        AsmBuilder &comment(std::string comment) override;

        AsmBuilder &newLine() override;
    };


} // brainlove

#endif //ASMBUILDER_H
