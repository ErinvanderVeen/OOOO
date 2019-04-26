#ifndef AI_H
#define AI_H

#include <inttypes.h>
#include <math.h>
#include <stdlib.h>

#include "state_t.h"

#define LIMIT       1
#define START_DEPTH 1
#define MAX_DEPTH   64

double evaluation(board_t board) {
    static const uint64_t CORNER_MASK = 0x8100000000000081ULL;
    double score = 0;

    // Reach the last move. If we are winning, assign a BIG score to us
    if (~(board.opponent | board.player) == 0)
        return (count(board.player) - count(board.opponent)) * (1 << 30);

    // A typical strategy is to give up discs during the "early" game since it will benefit us later.
    if (count(board.player | board.opponent) < 40)
        score += (count(board.opponent) - count(board.player));
    else
        score += (count(board.player) - count(board.opponent));

    // Corners are good
    score += (CORNER_MASK & board.player - (CORNER_MASK & board.opponent)) * 10;

    // Mobility is good
    score += count(get_valid_moves(board)) * 100;

    return score;
}

/**
 *
 *
 * @param board
 * @param depth
 * @param alpha
 * @param beta
 * @param player -  the current player to consider. 1 is the player, -1 is the opponent
 * @return
 */
double negamax(board_t board, uint64_t depth, double alpha, double beta, int player) {
    if (depth == 0 || ~(board.player | board.opponent) == 0)
        return player * evaluation(board);

    double value = -INFINITY;
    uint64_t valid = get_valid_moves(board);

    for (int i = 0; i < 64; ++i) {
        if (is_set(valid, i)) {
            board_t new_board = {.player = board.player, .opponent = board.opponent};
            do_move(&new_board, i);

            // We want the perspective of the other player in the recursive call
            switch_boards(&new_board);

            value = fmax(value, -negamax(new_board, depth - 1, -beta, -alpha, -player));
            alpha = fmax(alpha, value);

            if (alpha >= beta)
                break;
        }
    }
    return value;
}

uint8_t ai_turn(board_t board) {
    int8_t best_move = -1;
    double best_value = 0;
    uint64_t valid = get_valid_moves(board);

    if (count(valid) == 1) {
        for (uint8_t i = 0; i < 64; ++i) {
            if (is_set(valid, i))
                return i;
        }
    }

    uint8_t depth;
    long last_time;

    // TODO: The time granularity is seconds at the moment. Should be changed to milliseconds
    for (depth = START_DEPTH, last_time = time(NULL); (time(NULL) - last_time) < LIMIT && depth < MAX_DEPTH; depth++) {
        printf("Max depth: %d\n", depth);
        // TODO: Possible source for parallelization
        for (uint8_t i = 0; i < 64; ++i) {
            if (is_set(valid, i)) {
                double value = negamax(board, depth, -INFINITY, INFINITY, 1);
                if (best_value < value) {
                    best_value = value;
                    best_move = i;
                }
            }
        }
    }

    if (best_move == -1) {
        for (uint8_t i = 0; i < 64; ++i) {
            if (is_set(valid, i)) {
                best_move = i;
                break;
            }
        }
    }
    return best_move;
}

#endif
