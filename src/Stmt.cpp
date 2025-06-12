//
// Created by michael on 06.06.25.
//

#include "Stmt.h"

namespace goo {

    Conditional::~Conditional() {
        for (const auto it : stmts) {
            delete it;
        }
        stmts.clear();
    }


} // goo