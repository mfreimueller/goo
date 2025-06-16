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
            } else if (stmt->type == IF) {
                const auto conditional = std::static_pointer_cast<Conditional>(stmt);

                auto newStmts = run(conditional->stmts);
                optimizedStmts.emplace_back(new Conditional(stmt->column, stmt->line, newStmts));
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
            if (stmt->type == IF) {
                const auto conditional = std::static_pointer_cast<Conditional>(stmt);

                if (conditional->stmts.size() == 4) {
                    bool isValid = true;
                    int ptrShift = 0;
                    int offset = 0;
                    int matchedAll = INC_BYTE | INC_PTR | DEC_BYTE | DEC_PTR;

                    for (int idx = 0; idx < conditional->stmts.size(); idx++) {
                        if (conditional->stmts[idx]->type == INC_BYTE) {
                            const auto incByte = std::static_pointer_cast<IncrementByte>(conditional->stmts[idx]);
                            if (incByte->count != 1) {
                                isValid = false;
                                break;
                            }

                            matchedAll &= ~ INC_BYTE;

                            // If INC_PTR precedes INC_BYTE, we have a positive offset, otherwise a negative
                            if (idx > 0) {
                                if (conditional->stmts[idx - 1]->type == INC_PTR) {
                                    const auto incPtr = std::static_pointer_cast<IncrementPtr>(conditional->stmts[idx - 1]);
                                    offset = incPtr->count;
                                } else if (conditional->stmts[idx - 1]->type == DEC_PTR) {
                                    const auto decPtr = std::static_pointer_cast<DecrementPtr>(conditional->stmts[idx - 1]);
                                    offset = -decPtr->count;
                                } else {
                                    isValid = false;
                                    break;
                                }
                            } else {
                                isValid = false;
                                break;
                            }
                        } else if (conditional->stmts[idx]->type == DEC_BYTE) {
                            const auto decByte = std::static_pointer_cast<DecrementByte>(conditional->stmts[idx]);
                            if (decByte->count != 1) {
                                isValid = false;
                                break;
                            }

                            matchedAll &= ~ DEC_BYTE;
                        } else if (conditional->stmts[idx]->type == INC_PTR) {
                            const auto incPtr = std::static_pointer_cast<IncrementPtr>(conditional->stmts[idx]);
                            ptrShift += incPtr->count;

                            matchedAll &= ~ INC_PTR;
                        } else if (conditional->stmts[idx]->type == DEC_PTR) {
                            const auto decPtr = std::static_pointer_cast<DecrementPtr>(conditional->stmts[idx]);
                            ptrShift -= decPtr->count;

                            matchedAll &= ~ DEC_PTR;
                        } else {
                            isValid = false;
                            break;
                        }
                    }

                    if (isValid && ptrShift == 0 && matchedAll == 0) {
                        optimizedStmts.emplace_back(new Transfer(stmt->column, stmt->line, offset));
                        continue;
                    }
                }

                // recursively step into the conditional and try to optimize yet again
                auto newStmts = run(conditional->stmts);
                optimizedStmts.emplace_back(new Conditional(stmt->column, stmt->line, newStmts));
            }

            // If we cannot detect a reset statement, we simply add it untouched.
            optimizedStmts.push_back(stmt);
        }

        return optimizedStmts;
    }
}
