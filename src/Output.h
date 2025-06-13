//
// Created by michael on 13.06.25.
//

#ifndef OUTPUT_H
#define OUTPUT_H
#include "Pipeline.h"

namespace goo {
    /// A phase that accepts a StringPayload and prints it to std::cout.
    /// This is typically useful for debugging.
    class OutputPhase final : public Phase {
    public:
        explicit OutputPhase(Reporter &reporter) : Phase(reporter) {
        }

        std::shared_ptr<Payload> run(std::shared_ptr<Payload> payload) override;
    };
} // goo

#endif //OUTPUT_H
