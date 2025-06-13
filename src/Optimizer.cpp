//
// Created by michael on 12.06.25.
//

#include "Optimizer.h"

typedef std::vector<goo::OptimizationPass *> PassVector;

namespace goo {
    std::shared_ptr<Payload> Optimizer::run(const std::shared_ptr<Payload> payload) {
        // Add new passes to this list in the required order.
        const PassVector passes = {
            new GroupPass,
            new ResetPass,
            new TransferPass
        };

        const auto stmtPayload = std::static_pointer_cast<StmtPayload>(payload);

        auto stmts = stmtPayload->stmts;

        for (const auto &it: passes) {
            stmts = it->run(stmts);
            delete it;
        }

        return std::make_shared<StmtPayload>(StmtPayload{.stmts = stmts});
    }

    //
    // GroupPass
    //

    StmtVector GroupPass::run(const StmtVector &stmts) const { // NOLINT(*-no-recursion)
        StmtVector optimizedStmts;

        for (int idx = 0; idx < stmts.size(); idx++) {
            if (const auto &stmt = stmts[idx]; stmt->type < OUT) {
                auto groupedStmt = groupStmts(idx, stmts);
                optimizedStmts.emplace_back(groupedStmt);
            } else if (stmt->type == IF) {
                const auto conditional = std::static_pointer_cast<Conditional>(stmt);
                auto subStmts = run(conditional->stmts);

                optimizedStmts.emplace_back(new Conditional(stmt->column, stmt->line, subStmts));
            } else {
                optimizedStmts.push_back(stmt);
            }
        }

        return optimizedStmts;
    }

    std::shared_ptr<Stmt> GroupPass::groupStmts(int &idx, const StmtVector &stmts) const {
        auto stmt = stmts[idx];

        const int line = stmt->line;
        const int column = stmt->column;

        const bool isByteOp = stmt->type < INC_PTR;
        const TokenType increase = isByteOp ? INC_BYTE : INC_PTR;
        const TokenType decrease = isByteOp ? DEC_BYTE : DEC_PTR;

        int moves = 0;

        // We now iterate over all successive statements of the same type (ptr ops), where we track how many
        // times we increase or decrease. Additionally, we delete all statements to prevent memory leaks, as
        // we create a new grouped statements. At the end we decide, whether to create an increase or
        // a decrease statement.
        for (; idx < stmts.size(); idx++) {
            stmt = stmts[idx];
            if (stmt->type == increase) {
                moves++;
            } else if (stmt->type == decrease) {
                moves--;
            } else {
                // if we have no ptr operator anymore, we break the loop
                break;
            }
        }

        // We have to reduce the index by 1, because currently at idx there is an element that we couldn't group,
        // thus we have to process it normally.
        idx--;

        if (moves > 0) {
            if (isByteOp) {
                return std::make_shared<IncrementByte>(column, line, moves);
            } else {
                return std::make_shared<IncrementPtr>(column, line, moves);
            }
        } else if (moves < 0) {
            // As we have negative moves, we must negate them to have the positive value
            moves = -moves;

            if (isByteOp) {
                return std::make_shared<DecrementByte>(column, line, moves);
            } else {
                return std::make_shared<DecrementPtr>(column, line, moves);
            }
        }

        return stmt;
    }

    //
    // ResetPass
    //

    StmtVector ResetPass::run(const StmtVector &stmts) const {
        StmtVector optimizedStmts;

        // We iterate over all statements trying to detect a pattern of Conditional->Decrease. In this case we
        // insert a new reset statement in place of the conditional.
        for (int idx = 0; idx < stmts.size(); idx++) {
            const auto &stmt = stmts[idx];
            if (stmt->matches({IF, DEC_BYTE, FI})) {
                // Now that we know that we reset the current byte we also check for an assign statement directly
                // afterward: [-]+
                int initialValue = 0;

                if (idx + 1 < stmts.size() && stmts[idx + 1]->type == INC_BYTE) {
                    const auto incByte = std::static_pointer_cast<IncrementByte>(stmts[idx + 1]);
                    initialValue = incByte->count;

                    // increase index by one to skip this increase.
                    idx++;
                }

                optimizedStmts.emplace_back(new Reset(stmt->column, stmt->line, initialValue, 0));
                continue;
            }

            // If we cannot detect a reset statement, we simply add it untouched.
            optimizedStmts.push_back(stmt);
        }

        return optimizedStmts;
    }

    //
    // TransferPass
    //

    StmtVector TransferPass::run(const StmtVector &stmts) const {
        StmtVector optimizedStmts;

        // We iterate over all statements trying to detect a pattern of Conditional->Decrease. In this case we
        // insert a new reset statement in place of the conditional.
        for (const auto &stmt: stmts) {
            // Matched patterns: [>+<-], [->+<], [<+>-] or [>-<+>]
            if (stmt->matches({IF, DEC_BYTE, INC_PTR, INC_BYTE, DEC_PTR, FI}) ||
                stmt->matches({IF, INC_PTR, INC_BYTE, DEC_PTR, DEC_BYTE, FI}) ||
                stmt->matches({IF, DEC_PTR, INC_BYTE, INC_PTR, DEC_BYTE, FI}) ||
                stmt->matches({IF, INC_PTR, DEC_BYTE, DEC_PTR, INC_BYTE, INC_PTR, FI})) { // TODO: test this last combination
                const auto conditional = std::static_pointer_cast<Conditional>(stmt);

                int offset = 0;
                int addOffset = 0;
                int subOffset = 0;

                // We are only interested in direct copies. No [>++<-]. This is matter of a different optimization.
                // If we detect that we are or remove more than one bit at a time, we bail out.
                bool isValidCopy = true;

                // we track all ptr operations to calculate the offset, which we then use to write the correct copy
                // operation
                for (const auto &it: conditional->stmts) {
                    if (it->type == INC_PTR) {
                        const auto incPtr = std::static_pointer_cast<IncrementPtr>(it);
                        offset += incPtr->count;
                    } else if (it->type == DEC_PTR) {
                        const auto decPtr = std::static_pointer_cast<DecrementPtr>(it);
                        offset -= decPtr->count;
                    } else if (it->type == INC_BYTE) {
                        const auto incByte = std::static_pointer_cast<IncrementByte>(it);
                        if (incByte->count > 1) {
                            isValidCopy = false;
                            break;
                        }

                        addOffset = offset;
                    } else if (it->type == DEC_BYTE) {
                        const auto decByte = std::static_pointer_cast<DecrementByte>(it);
                        if (decByte->count > 1) {
                            isValidCopy = false;
                            break;
                        }

                        subOffset = offset;
                    }
                }

                // If we have a valid copy statement we create it. Otherwise, we skip this and add the conditional as is.
                if (isValidCopy) {
                    optimizedStmts.emplace_back(new Transfer(stmt->column, stmt->line, addOffset, subOffset));

                    if (offset > 0) {
                        optimizedStmts.emplace_back(new IncrementPtr(stmt->column, stmt->line, offset));
                    } else if (offset < 0) {
                        optimizedStmts.emplace_back(new DecrementPtr(stmt->column, stmt->line, offset));
                    }

                    continue;
                }
            }

            // If we cannot detect a reset statement, we simply add it untouched.
            optimizedStmts.push_back(stmt);
        }

        return optimizedStmts;
    }
}
