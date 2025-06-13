//
// Created by michael on 12.06.25.
//

#include "Optimizer.h"

namespace goo {
    std::shared_ptr<Payload> Optimizer::run(const std::shared_ptr<Payload> payload) {
        // reset state of optimizer to allow for reuse
        optimizedStmts.clear();

        const auto stmtPayload = std::static_pointer_cast<StmtPayload>(payload);

        const auto stmts = stmtPayload->stmts;
        const auto optimizedStmts = optimizeStmts(stmts);

        return std::make_shared<StmtPayload>(StmtPayload { .stmts = optimizedStmts });
    }

    std::vector<std::shared_ptr<Stmt>> Optimizer::optimizeStmts(const std::vector<std::shared_ptr<Stmt>> &stmts) {
        std::vector<std::shared_ptr<Stmt>> optimizedStmts;

        for (int idx = 0; idx < stmts.size(); idx++) {
            if (const auto& stmt = stmts[idx]; stmt->type < OUT) {
                auto grouped = groupStmts(idx, stmts);
                optimizedStmts.insert(optimizedStmts.end(), grouped.begin(), grouped.end());
            } else if (stmt->type == IF) {
                const auto conditional = std::static_pointer_cast<Conditional>(stmt);
                auto newStmts = optimizeStmts(conditional->stmts);

                optimizedStmts.emplace_back(new Conditional(stmt->column, stmt->line, newStmts));
            } else {
                optimizedStmts.push_back(stmt);
            }
        }

        return optimizedStmts;
    }

    std::vector<std::shared_ptr<Stmt>> Optimizer::groupStmts(int &idx, const std::vector<std::shared_ptr<Stmt> > &stmts) {
        std::vector<std::shared_ptr<Stmt>> optimizedStmts;

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
                optimizedStmts.push_back(std::make_shared<IncrementByte>(line, column, moves));
            } else {
                optimizedStmts.push_back(std::make_shared<IncrementPtr>(line, column, moves));
            }
        } else if (moves < 0) {
            // As we have negative moves, we must negate them to have the positive value
            moves = -moves;

            if (isByteOp) {
                optimizedStmts.push_back(std::make_shared<DecrementByte>(line, column, moves));
            } else {
                optimizedStmts.push_back(std::make_shared<DecrementPtr>(line, column, moves));
            }
        }

        return optimizedStmts;
    }
}
