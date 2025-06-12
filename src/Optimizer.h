//
// Created by michael on 12.06.25.
//

#ifndef OPTIMIZER_H
#define OPTIMIZER_H
#include <vector>

#include "Stmt.h"

namespace goo {
    /// A class that analyzes a list of statements and tries to group similar statements in an attempt to reduce the
    /// number of operations and thus optimizing the code.
    class Optimizer {
    public:
        /// Analyses a list of statements and creates an optimized list that groups duplicates, as well as (in the
        /// future) unreachable or redundant code.
        /// @param stmts A list of statements that are to be optimized. Nullptr-entries are being ignored.
        /// @return A list of optimized statements.
        std::vector<Stmt *> run(const std::vector<Stmt *> &stmts);

    private:
        void groupStmts(int &idx, const std::vector<Stmt *> &stmts, std::vector<Stmt *> &optimized_stmts);
    };
}

#endif //OPTIMIZER_H
