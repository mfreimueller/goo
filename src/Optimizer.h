//
// Created by michael on 12.06.25.
//

#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <vector>

#include "Optimizer.h"
#include "Payload.h"
#include "Pipeline.h"
#include "Stmt.h"

namespace goo {
    class OptimizationPass;

    typedef std::vector<std::shared_ptr<Stmt>> StmtVector;
    typedef std::vector<std::shared_ptr<OptimizationPass>> OptimizationVector;

    /// A class that analyzes a list of statements and tries to group similar statements in an attempt to reduce the
    /// number of operations and thus optimizing the code.
    class Optimizer final : public Phase {
    public:
        explicit Optimizer(Reporter &reporter) : Phase(reporter) {
        }

        /// Analyses a list of statements and creates an optimized list that groups duplicates, as well as (in the
        /// future) unreachable or redundant code.
        /// @param payload A list of statements that are to be optimized. Nullptr-entries are being ignored.
        /// @return A statement payload containing a list of optimized statements.
        std::shared_ptr<Payload> run(std::shared_ptr<Payload> payload) override;

    };

    /// An optimization pass that transforms a list of statements.
    class OptimizationPass {
    public:
        virtual ~OptimizationPass() = default;

        [[nodiscard]] virtual StmtVector run(const StmtVector &stmts) const = 0;
    };

    /// An optimization pass that matches patterns of repeating operations of type byte increase/decrease and
    /// pointer increase/decrease.
    class GroupPass final : public OptimizationPass {
    public:
        [[nodiscard]] StmtVector run(const StmtVector &stmts) const override;

    private:
        /// Groups a list of statements of a similar type (*BYTE or *PTR) into one single statement. The idx-param
        /// points to the first statement to group. After the execution of the method, the index points to the last
        /// element that was grouped, to allow for regular processing in the caller.
        /// @param idx The current index of stmts where grouping should begin.
        /// @param stmts A list of statements to group.
        /// @return A statement that groups a list of successive statements.
        std::shared_ptr<Stmt> groupStmts(int &idx, const StmtVector &stmts) const;
    };

    /// A pass that detect patterns of type [-] and replaces them with a reset statement. If the reset pattern is
    /// immediately followed by an INC_BYTE ( [-]+ ), the count of the INC_BYTE is used as initial value, otherwise 0.
    class ResetPass final : public OptimizationPass {
    public:
        [[nodiscard]] StmtVector run(const StmtVector &stmts) const override;
    };

    /// A pass that detects patterns of type [>+<-], [->+<], [<+>-] or [>-<+>] and replaces them with two reset statements which moves the
    /// value of the copier to the copy and clear the copies.
    class TransferPass : public OptimizationPass {
    public:
        [[nodiscard]] StmtVector run(const StmtVector &stmts) const override;

    protected:
        bool extract(const std::shared_ptr<Conditional> &conditional, int &offset, int &incCount) const;
    };

    /// A pass that detects patterns of type +[>+<-] or similar. Compared to TransferPass, this pass allows for more than
    /// one increase statements in the conditional.
    class MultiplyPass final : public TransferPass {
    public:
        [[nodiscard]] StmtVector run(const StmtVector &stmts) const override;
    };

}

#endif //OPTIMIZER_H
