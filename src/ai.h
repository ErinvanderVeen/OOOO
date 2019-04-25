#ifndef AI_H
#define AI_H

#include <inttypes.h>
#include <math.h>
#include <stdlib.h>

#include "debug.h"
#include "state_t.h"

/**
 * @param[in] The Player board
 * @param[in] The Opponent board
 * @param[out] What pieces should be flipped in case a coordinate is picked.
 *
 * @param[in,out] The coordinate of a valid move (0 - 63) or 64 in case no more valid moves exist
 */
void next_possible_move(uint8_t *coordinate, uint64_t ai_valid_moves) {
	while (*coordinate < 64) {
		//TODO: Replace column, row with a single coordinate. Related to issue #10
		if (is_piece(ai_valid_moves, *coordinate)) {
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
 */
double alphabeta(board_t board, uint64_t depth, double alpha, double beta, bool maximize) {
	debug_print("Depth: %" PRIu64 "\n", depth);
	if (depth == 0)
		return evaluation(board);

	double val;

	uint8_t move = 0;
	uint64_t ai_valid_moves = get_valid_moves(board);

	if (maximize) {
		next_possible_move(&move, ai_valid_moves);
		val = -INFINITY;

		while (move != 64) {
			board_t new_board = {
				.player = board.player,
				.opponent = board.opponent
			};

			// Update board states
			do_move(&board, move);

			val = fmax(val, alphabeta(new_board, depth - 1, alpha, beta, false));
			alpha = fmax(alpha, val);

			if (alpha >= beta)
				break;

			move++;
			next_possible_move(&move, ai_valid_moves);
		}
		return val;
	} else {
		next_possible_move(&move, ai_valid_moves);
		val = INFINITY;

		while (move != 64) {
			board_t new_board = {
				.player = board.player,
				.opponent = board.opponent
			};

			// Update board states
			do_move(&board, move);

			val = fmin(val, alphabeta(new_board, depth - 1, alpha, beta, true));
			beta = fmin(beta, val);

			if (alpha > beta)
				break;

			move++;
			next_possible_move(&move, ai_valid_moves);
		}
		return val;
	}
}

uint8_t ai_turn(board_t board) {
	uint8_t best_move = 0;
	double best_val = -INFINITY;

	uint8_t move = 0;
	double val;

	uint64_t ai_valid_moves = get_valid_moves(board);

	next_possible_move(&move, ai_valid_moves);

	while (move != 64) {

		// We don't make a copy of to_flip, because we don't have to
		// it is updated after every next_possible_move anyway
		board_t new_board = {
			.player = board.player,
			.opponent = board.opponent
		};

		// Update board states
		do_move(&board, move);

		// Perform alphabeta on all children
		if ((val = alphabeta(new_board, 20, -INFINITY, INFINITY, false)) > best_val) {
			best_move = move;
			best_val = val;
		}

		move++;
		next_possible_move(&move, ai_valid_moves);
	}

	return best_move;
}

#endif
