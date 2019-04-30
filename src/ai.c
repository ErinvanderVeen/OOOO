#include "ai.h"

#include <assert.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#include "debug.h"
#include "eval_hashmap.h"

#define LIMIT       1
#define START_DEPTH 1
#define MAX_DEPTH   64

static uint64_t nodes;

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

double negamax(board_t board, uint64_t depth, double alpha, double beta, int8_t player) {
	// Lookup board in hash table
	board_eval_t *eval = find_eval(board);
	if (eval != NULL && eval->depth > depth)
		return eval->value;

	// Depth 0, use evaluation function
	if (depth == 0 || ~(board.player | board.opponent) == 0)
		return player * evaluation(board);

	double value = -INFINITY;
	uint64_t valid = get_valid_moves(board);

	for (uint8_t i = 0; i < 64; ++i) {
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

	// Place/update in hashtable
	if (eval == NULL) {
		eval = malloc(sizeof(*eval));
		eval->board = board;
		// Note, we add first, based on only the board
		// after that, we set the values
		add_eval(eval);
	}
	eval->value = value;
	eval->depth = depth;

	nodes++;
	return value;
}

uint8_t ai_turn(board_t board) {

	init_map();

	uint64_t valid = get_valid_moves(board);
	nodes = 0;

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
		debug_print("Max depth: %" PRIu8 "\n", depth);

#pragma omp parallel
		{
			for (uint8_t i = 0; i < 64; ++i) {
				if (is_set(valid, i)) {
					board_t new_board = {.player = board.player, .opponent = board.opponent};
					do_move(&new_board, i);

					// We want the perspective of the other player in the recursive call
					switch_boards(&new_board);

					negamax(new_board, depth, -INFINITY, INFINITY, 1);
				}
			}
#pragma omp barrier
		}
	}

	// Retrieve the best move from the hashtable
	double best_value = -INFINITY;
	double best_move = -1;
	for (uint8_t i = 0; i < 64; ++i) {
		if (is_set(valid, i)) {
			board_t new_board = {.player = board.player, .opponent = board.opponent};
			do_move(&new_board, i);
			switch_boards(&new_board);

			board_eval_t *eval = find_eval(new_board);
			if (eval != NULL) {
				if (-eval->value > best_value) {
					best_value = -eval->value;
					best_move = i;
				}
			}
		}
	}

	printf("Nodes/s: %f\n", (double) nodes / LIMIT);

	assert(best_move != -1);

	free_map();

	return best_move;
}
