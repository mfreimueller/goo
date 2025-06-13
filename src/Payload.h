//
// Created by michael on 13.06.25.
//

#ifndef PAYLOAD_H
#define PAYLOAD_H
#include <memory>
#include <string>
#include <vector>

#include "Stmt.h"
#include "Token.h"

namespace goo {

    /// A base struct used for different payload implementations that are passed between compiler phases.
    struct Payload {};

    /// A payload containing a filepath that typically contains code.
    struct FilePayload : Payload {
        const std::string filepath;
    };

    /// A payload that contains a string value, typically representing code.
    struct StringPayload : Payload {
        const std::string value;
    };

    /// A payload containing a list of tokens.
    struct TokenPayload : Payload {
        const std::vector<std::shared_ptr<Token>> tokens;
    };

    /// A payload containing a list of statements that may or may not be optimized.
    struct StmtPayload : Payload {
        const std::vector<std::shared_ptr<Stmt>> stmts;
    };

}

#endif //PAYLOAD_H
