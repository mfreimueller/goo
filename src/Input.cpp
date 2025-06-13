//
// Created by michael on 13.06.25.
//

#include "Input.h"

#include <fstream>

#include "Reporter.h"

namespace goo {
    std::shared_ptr<Payload> FileInput::run(std::shared_ptr<Payload> payload) {
        const auto filePayload = std::static_pointer_cast<FilePayload>(payload);

        auto ifs = std::ifstream(filePayload->filepath);
        const auto fileContent = std::string(std::istreambuf_iterator{ifs}, {});

        reporter.setCode(fileContent);

        return std::make_shared<StringPayload>(StringPayload{.value = fileContent});
    }

    std::shared_ptr<Payload> StringInput::run(std::shared_ptr<Payload> payload) {
        const auto stringPayload = std::static_pointer_cast<StringPayload>(payload);
        reporter.setCode(stringPayload->value);

        // as we expected only StringPayload as input, we simply return it.
        return payload;
    }
} // goo
