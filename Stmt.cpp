//
// Created by michael on 06.06.25.
//

#include "Stmt.h"

namespace brainlove {

    Conditional::~Conditional() {
        for (const auto it : stmts) {
            delete it;
        }
        stmts.clear();
    }


} // brainlove