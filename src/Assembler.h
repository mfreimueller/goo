//
// Created by michael on 13.06.25.
//

#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include <utility>

#include "Pipeline.h"

namespace goo {
    struct AssemblerConfig {
        const bool debugBuild;
        const bool verbose;
        const std::string outputFile;
    };

    class Assembler final : public Phase {
    private:
        AssemblerConfig config;
    public:
        explicit Assembler(AssemblerConfig config, Reporter &reporter) : Phase(reporter), config(std::move(config)) {
        }

        /// Receives a payload of type StringPayload, writes the content of the payload to a temporary file and finally
        /// invokes nasm to create an ELF object-file.
        std::shared_ptr<Payload> run(std::shared_ptr<Payload> payload) override;

    private:
        bool writeToTmpFile(const std::string &content, std::string &fileName) const;
    };
} // goo

#endif //ASSEMBLER_H
