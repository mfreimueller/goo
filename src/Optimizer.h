//
// Created by michael on 12.06.25.
//

#ifndef OPTIMIZER_H
#define OPTIMIZER_H
#include <vector>

#include "Payload.h"
#include "Pipeline.h"
#include "Stmt.h"

namespace goo {
    /// A class that analyzes a list of statements and tries to group similar statements in an attempt to reduce the
    /// number of operations and thus optimizing the code.
    class Optimizer : public Phase {
        std::vector<std::shared_ptr<Stmt>> optimizedStmts;

    public:
        explicit Optimizer(Reporter &reporter) : Phase(reporter) {
        }

        /// Analyses a list of statements and creates an optimized list that groups duplicates, as well as (in the
        /// future) unreachable or redundant code.
        /// @param stmts A list of statements that are to be optimized. Nullptr-entries are being ignored.
        /// @return A list of optimized statements.
        std::shared_ptr<Payload> run(std::shared_ptr<Payload> payload) override;

    private:
        std::vector<std::shared_ptr<Stmt>> optimizeStmts(const std::vector<std::shared_ptr<Stmt>> &stmts);

        std::vector<std::shared_ptr<Stmt>> groupStmts(int &idx, const std::vector<std::shared_ptr<Stmt>> &stmts);
    };
}

#endif //OPTIMIZER_H
