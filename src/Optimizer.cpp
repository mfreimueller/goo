//
// Created by michael on 12.06.25.
//

#include "Optimizer.h"

namespace goo {

    std::vector<Stmt *> Optimizer::run(const std::vector<Stmt *> &stmts) {
        std::vector<Stmt *> optimizedStmts;

        for (int idx = 0; idx < stmts.size(); idx++) {
            if (auto stmt = stmts[idx]; stmt->type < OUT) {
                groupStmts(idx, stmts, optimizedStmts);
            } else {
                optimizedStmts.push_back(stmt);
            }
        }

        return optimizedStmts;
    }

    void Optimizer::groupStmts(int &idx, const std::vector<Stmt *> &stmts, std::vector<Stmt *> &optimized_stmts) {
        auto stmt = stmts[idx];

        const int line = stmt->line;
        const int column = stmt->column;

        const bool isByteOp = stmt->type < INC_PTR;
        const TokenType increase = isByteOp ? TokenType::INC_BYTE : TokenType::INC_PTR;
        const TokenType decrease = isByteOp ? TokenType::DEC_BYTE : TokenType::DEC_PTR;

        int moves = 0;

        // We now iterate over all successive statements of the same type (ptr ops), where we track how many
        // times we increase or decrease. Additionally, we delete all statements to prevent memory leaks, as
        // we create a new grouped statements. At the end we decide, whether to create an increase or
        // a decrease statement.
        for (; idx < stmts.size(); idx++) {
            stmt = stmts[idx];
            if (stmt->type == increase) {
                moves++;
                delete stmts[idx];
            } else if (stmt->type == decrease) {
                moves--;
                delete stmts[idx];
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
                optimized_stmts.push_back(new IncrementByte(line, column, moves));
            } else {
                optimized_stmts.push_back(new IncrementPtr(line, column, moves));
            }

        } else if (moves < 0) {
            // As we have negative moves, we must negate them to have the positive value
            moves = -moves;

            if (isByteOp) {
                optimized_stmts.push_back(new DecrementByte(line, column, moves));
            } else {
                optimized_stmts.push_back(new DecrementPtr(line, column, moves));
            }
        }
    }

}