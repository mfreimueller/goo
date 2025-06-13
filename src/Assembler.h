//
// Created by michael on 13.06.25.
//

#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include "Pipeline.h"

namespace goo {

class Assembler final : public Phase {
public:
    explicit Assembler(Reporter &reporter) : Phase(reporter) {}

    /// Receives a payload of type StringPayload, writes the content of the payload to a temporary file and finally
    /// invokes nasm to create an ELF object-file.
    std::shared_ptr<Payload> run(std::shared_ptr<Payload> payload) override;

private:
    bool writeToTmpFile(const std::string& content, std::string &fileName) const;
};

} // goo

#endif //ASSEMBLER_H
