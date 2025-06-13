//
// Created by michael on 06.06.25.
//

#include "Stmt.h"

namespace goo {
    bool Stmt::matches(const std::vector<TokenType> pattern) const {
        return (pattern.size() == 1 && pattern[0] == type);
    }

    bool Conditional::matches(const std::vector<TokenType> pattern) const {
        std::vector<TokenType> stmtPattern;
        stmtPattern.reserve(stmts.size() + 2);
        stmtPattern.push_back(IF);

        for (const auto &it : stmts) {
            if (it != nullptr) {
                stmtPattern.push_back(it->type);
            }
        }

        stmtPattern.push_back(FI);

        return stmtPattern == pattern;
    }
} // goo