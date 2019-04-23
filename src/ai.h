#ifndef AI_H
#define AI_H

#include <inttypes.h>
#include <math.h>
#include <stdlib.h>

#include "debug.h"
#include "state_t.h"

#ifdef PERFORMANCE
extern uint64_t nodes_explored;
#endif

uint8_t count_pieces(uint64_t board) {
	uint8_t count = 0;
	while (board) {
		count += board & 1;
		board >>= 1;
	}
	return count;
}

/**
 * @param[in] The Player board
 * @param[in] The Opponent board
 * @param[out] What pieces should be flipped in case a coordinate is picked.
 *
 * @param[in,out] The coordinate of a valid move (0 - 63) or 64 in case no more valid moves exist
 */
void next_possible_move(board_t board, uint64_t to_flip[64], uint8_t *coordinate) {
	while (*coordinate < 64) {
		//TODO: Replace column, row with a single coordinate. Related to issue #10
		if (is_valid_move(board, *coordinate, to_flip)) {
			return;
		}
		(*coordinate)++;
	}
	*coordinate = 64;
}

// Placeholder evaluation function
double evaluation(board_t board) {
	double c_player = (double) count_pieces(board.player);
	double c_opponent = (double) count_pieces(board.opponent);

	return c_player - c_opponent;
}

/**
 * alphabeta
 *
 * TODO: to_flip can be passed to this function such that we don't have to allocate
 */
double alphabeta(board_t board, uint64_t depth, double alpha, double beta, bool maximize) {
	debug_print("Depth: %" PRIu64 "\n", depth);
	if (depth == 0)
		return evaluation(board);

	uint64_t to_flip[64];

	double val;

	uint8_t move = 0;

#ifdef PERFORMANCE
	nodes_explored++;
#endif

	if (maximize) {
		next_possible_move(board, to_flip, &move);
		val = -INFINITY;

		while (move != 64) {
			board_t new_board = {
				.player = board.player,
				.opponent = board.opponent
			};

			// Update board states
			do_move(&board, move, to_flip);

			val = fmax(val, alphabeta(new_board, depth - 1, alpha, beta, false));
			alpha = fmax(alpha, val);

			if (alpha >= beta)
				break;

			move++;
			next_possible_move(board, to_flip, &move);
		}
		return val;
	} else {
		next_possible_move(board, to_flip, &move);
		val = INFINITY;

		while (move != 64) {
			board_t new_board = {
				.player = board.player,
				.opponent = board.opponent
			};

			// Update board states
			do_move(&board, move, to_flip);

			val = fmin(val, alphabeta(new_board, depth - 1, alpha, beta, true));
			beta = fmin(beta, val);

			if (alpha > beta)
				break;

			move++;
			next_possible_move(board, to_flip, &move);
		}
		return val;
	}
}

uint8_t ai_turn(board_t board) {
	uint64_t to_flip[64];
	uint8_t best_move = 0;
	double best_val = -INFINITY;

	uint8_t move = 0;
	double val;

	next_possible_move(board, to_flip, &move);

	while (move != 64) {

		// We don't make a copy of to_flip, because we don't have to
		// it is updated after every next_possible_move anyway
		board_t new_board = {
			.player = board.player,
			.opponent = board.opponent
		};

		// Update board states
		do_move(&board, move, to_flip);

		// Perform alphabeta on all children
		if ((val = alphabeta(new_board, 8, -INFINITY, INFINITY, false)) > best_val) {
			best_move = move;
			best_val = val;
		}

		move++;
		next_possible_move(board, to_flip, &move);
	}

	return best_move;
}

#endif
