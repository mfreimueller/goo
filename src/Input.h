//
// Created by michael on 13.06.25.
//

#ifndef INPUT_H
#define INPUT_H

#include "Payload.h"
#include "Pipeline.h"

namespace goo {

    /// An initial compiler phase that reads a file and returns its contents as payload for the next phase.
    class FileInput final : public Phase {
    public:
        explicit FileInput(Reporter &reporter) : Phase(reporter) {
        }

        std::shared_ptr<Payload> run(std::shared_ptr<Payload> payload) override;
    };

    /// A dummy compiler phase that forwards a string to the next phase.
    class StringInput final : public Phase {
    public:
        explicit StringInput(Reporter &reporter) : Phase(reporter) {
        }

        std::shared_ptr<Payload> run(std::shared_ptr<Payload> payload) override;
    };
} // goo

#endif //INPUT_H
