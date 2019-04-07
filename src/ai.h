#ifndef AI_H
#define AI_H

#include <inttypes.h>
#include <math.h>
#include <stdlib.h>

#include "debug.h"
#include "state_t.h"

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
void next_possible_move(uint64_t ai_player_b, uint64_t ai_opponent_b, uint64_t to_flip[8][8], uint8_t *coordinate) {
	while (*coordinate < 64) {
		//TODO: Replace column, row with a single coordinate. Related to issue #10
		if (is_valid_move(ai_player_b, ai_opponent_b, *coordinate % 8, *coordinate / 8, to_flip)) {
			return;
		}
		(*coordinate)++;
	}
	*coordinate = 64;
}

// Placeholder evaluation function
double evaluation(uint64_t ai_player_b, uint64_t ai_opponent_b) {
	double c_player = (double) count_pieces(ai_player_b);
	double c_opponent = (double) count_pieces(ai_opponent_b);

	return c_player - c_opponent;
}

/**
 * minimax
 *
 * TODO: to_flip can be passed to this function such that we don't have to allocate
 */
double minimax(uint64_t ai_player_b, uint64_t ai_opponent_b, uint64_t depth, bool maximize) {
	debug_print("Depth: %" PRIu64 "\n", depth);
	if (depth == 0)
		return evaluation(ai_player_b, ai_opponent_b);

	double best_val;
	uint64_t to_flip[8][8];

	double val;

	uint8_t move = 0;

	if (maximize) {
		next_possible_move(ai_player_b, ai_opponent_b, to_flip, &move);
		best_val = -INFINITY;

		while (move != 64) {
			uint64_t new_ai_player_b = ai_player_b;
			uint64_t new_ai_opponent_b = ai_opponent_b;

			// Update board states
			do_move(&new_ai_player_b, &new_ai_opponent_b, move % 8, move / 8, to_flip);

			if ((val = minimax(new_ai_player_b, new_ai_opponent_b, depth - 1, false)) > best_val)
				best_val = val;

			move++;
			next_possible_move(ai_player_b, ai_opponent_b, to_flip, &move);
		}
		return val;
	} else {
		next_possible_move(ai_opponent_b, ai_player_b, to_flip, &move);
		best_val = INFINITY;

		while (move != 64) {
			uint64_t new_ai_player_b = ai_player_b;
			uint64_t new_ai_opponent_b = ai_opponent_b;

			// Update board states
			do_move(&new_ai_opponent_b, &new_ai_player_b, move % 8, move / 8, to_flip);

			if ((val = minimax(new_ai_player_b, new_ai_opponent_b, depth - 1, true)) < best_val)
				best_val = val;

			move++;
			next_possible_move(ai_opponent_b, ai_player_b, to_flip, &move);
		}
		return val;
	}
}

uint8_t ai_turn(uint64_t ai_player_b, uint64_t ai_opponent_b) {
	uint64_t to_flip[8][8];
	uint8_t best_move = 0;
	double best_val = -INFINITY;

	uint8_t move = 0;
	double val;

	next_possible_move(ai_player_b, ai_opponent_b, to_flip, &move);

	while (move != 64) {

		// We don't make a copy of to_flip, because we don't have to
		// it is updated after every next_possible_move anyway
		uint64_t new_ai_player_b = ai_player_b;
		uint64_t new_ai_opponent_b = ai_opponent_b;

		// Update board states
		do_move(&new_ai_player_b, &new_ai_opponent_b, move % 8, move / 8, to_flip);

		// Perform minimax on all children
		if ((val = minimax(new_ai_player_b, new_ai_opponent_b, 5, false)) > best_val) {
			best_move = move;
			best_val = val;
		}

		move++;
		next_possible_move(ai_player_b, ai_opponent_b, to_flip, &move);
	}

	return best_move;
}

#endif
