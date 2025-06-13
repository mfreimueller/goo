//
// Created by michael on 13.06.25.
//

#include "Output.h"

#include <iostream>

namespace goo {

    std::shared_ptr<Payload> OutputPhase::run(const std::shared_ptr<Payload> payload) {
        const auto stringPayload = std::static_pointer_cast<StringPayload>(payload);
        std::cout << stringPayload->value << std::endl;

        return nullptr;
    }

} // goo