#include "ai.hpp"

#include <assert.h>
#include <math.h>
#include <omp.h>
#include <stdbool.h>
#include <time.h>

#include "debug.hpp"
#include "eval_hashmap.hpp"

#define TIMELIMIT   100 //In ms
#define START_DEPTH 1
#define MAX_DEPTH   64

#ifdef METRICS
static uint64_t branches;
static uint64_t branches_evaluated;
#endif

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
	score += ((CORNER_MASK & board.player) - (CORNER_MASK & board.opponent)) * 10;

	// Mobility is good
	score += count(get_valid_moves(board)) * 100;

	return score;
}

/**
 * This function fetches the best child from the hashmap
 * It is important that at least one child has a value in the hashtable
 */
static int8_t get_best_move(board_t board, uint64_t valid) {
	double best_value = -INFINITY;

	// Set the least significant set bit in the valid bitmask as default move
	int8_t best_move = __builtin_ffsll(valid) - 1;

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
	return best_move;
}

double negamax(board_t board, uint64_t depth, double alpha, double beta, int8_t player) {
#ifdef METRICS
	uint8_t children_evaluated = 0;
#endif
	// If we should be done with regards to time,
	// end this evaluation
	// Note: It does not matter what we return, because as soon as finished is
	// set to true, all results are disregarded
	if (get_time_ms() >= end_time_ms) {
		finished = true;
		return -INFINITY;
	}

	// Lookup board in hash table. We have to switch the board in order to get
	// the correct hash since the hash takes color into consideration.
	board_eval_t *eval = NULL;
	if (player == 1)
		eval = find_eval(board);
	else {
		switch_boards(&board);
		eval = find_eval(board);
		switch_boards(&board);
	}

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

#ifdef METRICS
			children_evaluated++;
#endif

			if (alpha >= beta)
				break;
		}
	}

	// Prematurely end, do not update hashtable
	if (finished)
		return value;

	// Place/update in hashtable
	if (eval == NULL) {
		eval = (board_eval_t *) calloc(sizeof(*eval), 1);
		eval->board.opponent = player == 1 ? board.player : board.opponent;
		eval->board.player = player == 1 ? board.opponent : board.player;
		// Note, we add first, based on only the board
		// after that, we set the values
		add_eval(eval);
	}
	eval->value = value;
	eval->depth = depth;

#ifdef METRICS
	// Ensure that we don't get mixed up print data (hampers performance)
	uint8_t children = count(valid);
	branches += children;
	branches_evaluated += children_evaluated;
#endif

	nodes++;
	return value;
}

int8_t ai_turn(board_t board) {
#ifdef DEBUG
	long start_time_ms = get_time_ms();
	end_time_ms = start_time_ms + TIMELIMIT;
#else
	end_time_ms = get_time_ms() + TIMELIMIT;
#endif

#ifdef METRICS
	branches = 0;
	branches_evaluated = 0;
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

	// Calculate how many moves there are left. It lets us skip evaluating
	// unnecessary depths in the late game
	uint8_t moves_left = count(~(board.player | board.opponent));
	uint8_t depth;

	for (depth = START_DEPTH; !finished && depth < MAX_DEPTH && depth <= moves_left; depth++) {
		debug_print("Max depth: %" PRIu8 "\n", depth);

#pragma omp parallel for
		for (uint8_t i = 0; i < 64; ++i) {
			if (is_set(valid, i)) {
				board_t new_board = {.player = board.player, .opponent = board.opponent};
				do_move(&new_board, i);

				// We want the perspective of the other player in the recursive call
				switch_boards(&new_board);
				negamax(new_board, depth, -INFINITY, INFINITY, 1);
			}
		}
#ifdef METRICS
		printf("DEPTH: %" PRIu8 "\n", depth);
		char c, r;
		from_coordinate(get_best_move(board, valid), &c, &r);
		printf("\t Best move (so far): %c%c\n", c, r);
#endif
	}

	// Retrieve the best move from the hashtable
	int8_t best_move = get_best_move(board, valid);

	printf("Nodes/s: %f\n", (double) nodes / (TIMELIMIT / 1000.0));

#ifdef METRICS
	printf("AI:\n");
	printf("\t Start Depth: %" PRIu8 "\n", START_DEPTH);
	printf("\t Reached Depth: %" PRIu8 "\n", depth);
	printf("\t Branches: %" PRIu64 "\n", branches);
	printf("\t Branches explored: %" PRIu64 "\n", branches_evaluated);
	printf("\t Branches pruned: %" PRIu64 "\n", branches - branches_evaluated);
	printf("\t %% Pruned: %f\n", 100.0 * ((double) branches - (double) branches_evaluated) / branches);
	print_map();
#endif

	return best_move;
}
