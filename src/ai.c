#include "ai.h"

#include <assert.h>
#include <math.h>
#include <omp.h>
#include <stdbool.h>
#include <time.h>

#include "debug.h"
#include "eval_hashmap.h"

#define TIMELIMIT   100 //In ms
#define START_DEPTH 1
#define MAX_DEPTH   64

static uint64_t nodes;
static long end_time_ms;
static bool finished;

static long get_time_ms(void) {
	struct timespec spec;

	clock_gettime(CLOCK_REALTIME, &spec);
	return spec.tv_nsec / 1.0e6 + spec.tv_sec * 1000;
}

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
	// If we should be done with regards to time,
	// end this evaluation
	// Note: It does not matter what we return, because as soon as finished is
	// set to true, all results are disregarded
	if (get_time_ms() >= end_time_ms) {
		finished = true;
		return -INFINITY;
	}

	// Lookup board in hash table
	board_eval_t *eval = find_eval(board);
	if (eval != NULL && eval->depth > depth)
		return eval->value;

	// Depth 0, use evaluation function
	if (depth == 0 || ~(board.player | board.opponent) == 0)
		return player * evaluation(board);

	double value = -INFINITY;
	uint64_t valid = get_valid_moves(board);

	for (uint8_t i = 0; i < 64 && !finished; ++i) {
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

	// Prematurely end, do not update hashtable
	if (finished)
		return value;

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
#ifdef DEBUG
	long start_time_ms = get_time_ms();
	end_time_ms = start_time_ms + TIMELIMIT;
#else
	end_time_ms = get_time_ms() + TIMELIMIT;
#endif

	finished = false;

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

	// TODO: The time granularity is seconds at the moment. Should be changed to milliseconds
	for (depth = START_DEPTH; !finished && depth < MAX_DEPTH; depth++) {
		debug_print("Max depth: %" PRIu8 "\n", depth);

#ifdef PARALLEL
#pragma omp parallel
#endif
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
#ifdef PARALLEL
#pragma omp barrier
#endif
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

#ifdef DEBUG
	printf("Nodes/s: %f\n", (double) nodes / (TIMELIMIT / 1000.0));
#endif

	assert(best_move != -1);

	free_map();

	return best_move;
}
