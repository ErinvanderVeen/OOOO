#include "ai.hpp"

#include <assert.h>
#include <math.h>
#include <omp.h>
#include <stdbool.h>
#include <time.h>

#include "../lib/debug.hpp"
#include "../lib/eval_hashmap.hpp"

#define START_DEPTH 1
uint64_t time_limit; //In ms
uint8_t max_depth = 64;

#ifdef METRICS
static uint64_t branches = 0;
static uint64_t branches_evaluated = 0;
static uint64_t levels_evaluated = 0;
static uint64_t nr_moves = 0;
static uint64_t unique_nodes = 0;
static uint64_t nodes_evaluated = 0;
#endif

static uint64_t nodes = 0;
static long end_time_ms;
static bool finished;

static int8_t weights[64] = {
	20, -3, 11, 8, 8, 11, -3, 20,
	-3, -7, -4, 1, 1, -4, -7, -3,
	11, -4, 2, 2, 2, 2, -4, 11,
	8, 1, 2, -3, -3, 2, 1, 8,
	8, 1, 2, -3, -3, 2, 1, 8,
	11, -4, 2, 2, 2, 2, -4, 11,
	-3, -7, -4, 1, 1, -4, -7, -3,
	20, -3, 11, 8, 8, 11, -3, 20};

void set_max_depth(uint8_t depth) {
	max_depth = depth;
}

static long get_time_ms(void) {
	struct timespec spec;

	clock_gettime(CLOCK_REALTIME, &spec);
	return spec.tv_nsec / 1.0e6 + spec.tv_sec * 1000;
}

double evaluation(board_t board) {
	// Reached the last move. If we are winning, assign a BIG score to us
	if (~(board.opponent | board.player) == 0)
		return (count(board.player) - count(board.opponent)) * 8192;

	double a, b, c;
	double my_discs = 0;
	double opp_discs = 0;
	double weight_score = 0;

	for (int i = 0; i < 64; ++i) {
		if (is_set(board.player, i)) {
			weight_score += weights[i];
			my_discs++;
		} else if (is_set(board.opponent, i)) {
			weight_score -= weights[i];
			opp_discs++;
		}
	}
	if (my_discs > opp_discs)
		a = (100.0 * my_discs) / (my_discs + opp_discs);
	else if (my_discs < opp_discs)
		a = -(100.0 * opp_discs) / (my_discs + opp_discs);
	else a = 0;

	my_discs = opp_discs = 0;
	if (is_set(board.player, 0)) my_discs++;
	else if (is_set(board.opponent, 0)) opp_discs++;
	if (is_set(board.player, 7)) my_discs++;
	else if (is_set(board.opponent, 7)) opp_discs++;
	if (is_set(board.player, 56)) my_discs++;
	else if (is_set(board.opponent, 56)) opp_discs++;
	if (is_set(board.player, 63)) my_discs++;
	else if (is_set(board.opponent, 63)) opp_discs++;
	b = 25 * (my_discs - opp_discs);

	uint8_t player_mob = count(get_valid_moves(board));
	switch_boards(&board);
	uint8_t opponent_mob = count(get_valid_moves(board));
	switch_boards(&board);

	if (player_mob > opponent_mob)
		c = (100.0 * player_mob) / (player_mob + opponent_mob);
	else if (player_mob < opponent_mob)
		c = -(100.0 * opponent_mob) / (player_mob + opponent_mob);
	else c = 0;

	return (10 * a) + (801.724 * b) + (78.922 * c) + (10 * weight_score);
}

/**
 * This function fetches the best child from the hashmap
 * It is important that at least one child has a value in the hashtable
 */
static int8_t get_best_move(board_t board, uint64_t valid) {
	double best_value = -INFINITY;

	// Set the least significant set bit in the valid bitmask as default move
	int8_t best_move = __builtin_ffsl(valid) - 1;

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

	nodes++;

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

	if (eval != NULL && eval->depth >= depth)
		return eval->value;

	// Depth 0, use evaluation function
	if (depth == 0 || ~(board.player | board.opponent) == 0)
		return player * evaluation(board);

	double value = -INFINITY;
	uint64_t valid = get_valid_moves(board);

	uint8_t best_move = 64;

	// MOVE ORDERING
	if (eval != NULL) {
		best_move = eval->best_move;
		if (is_set(valid, best_move)) {
			board_t new_board = {.player = board.player, .opponent = board.opponent};
			do_move(&new_board, best_move);

			// We want the perspective of the other player in the recursive call
			switch_boards(&new_board);

			value = -negamax(new_board, depth - 1, -beta, -alpha, -player);
			alpha = fmax(alpha, value);

#ifdef METRICS
			children_evaluated++;
#endif
		}
	}

	for (uint8_t i = 0; !finished && i < 64; ++i) {
		if (is_set(valid, i) && i != best_move) {
			board_t new_board = {.player = board.player, .opponent = board.opponent};
			do_move(&new_board, i);

			// We want the perspective of the other player in the recursive call
			switch_boards(&new_board);

			double new_value = -negamax(new_board, depth - 1, -beta, -alpha, -player);
			if (new_value > value) {
				best_move = i;
				value = new_value;
			}

			alpha = fmax(alpha, new_value);

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

#ifdef METRICS
	// Ensure that we don't get mixed up print data (hampers performance)
	uint8_t children = count(valid);
	branches += children;
	branches_evaluated += children_evaluated;
#endif

	// Lookup board in hash table (again)
	if (player == 1)
		eval = find_eval(board);
	else {
		switch_boards(&board);
		eval = find_eval(board);
		switch_boards(&board);
	}
#ifdef METRICS
	nodes_evaluated++;
#endif
	if (eval != NULL && eval->depth >= depth) {
		return eval->value;
	} else if (eval == NULL) {
		eval = (board_eval_t *) malloc(sizeof(*eval));
		eval->board.player = player == 1 ? board.player : board.opponent;
		eval->board.opponent = player == 1 ? board.opponent : board.player;

		// Note, we add first, based on only the board
		// after that, we set the values
		add_eval(eval);
	}
	eval->value = value;
	eval->depth = depth;
	eval->best_move = best_move;

#ifdef METRICS
	unique_nodes++;
#endif

	return value;
}

int8_t ai_turn(board_t board, uint64_t time_ms) {
	time_limit = time_ms;
#ifdef DEBUG
	long start_time_ms = get_time_ms();
	end_time_ms = start_time_ms + time_limit;
#else
	end_time_ms = get_time_ms() + time_limit;
#endif

	finished = false;

	init_map();

	uint64_t valid = get_valid_moves(board);
#ifndef METRICS
	nodes = 0;
#endif

	if (count(valid) == 1) {
		for (uint8_t i = 0; i < 64; ++i) {
			if (is_set(valid, i))
				return i;
		}
	}

#ifdef PARALLEL
	uint8_t depth_inc = omp_get_max_threads();
#else
	uint8_t depth_inc = 1;
#endif

	// Calculate how many moves there are left. It lets us skip evaluating
	// unnecessary depths in the late game
	uint8_t moves_left = count(~(board.player | board.opponent));

	for (uint8_t depth = START_DEPTH; !finished && depth < max_depth && depth <= moves_left; depth += depth_inc) {
		debug_print("Max depth: %" PRIu8 "\n", depth);

		for (uint8_t i = 0; !finished && i < 64; ++i) {
			if (is_set(valid, i)) {
				board_t new_board = {.player = board.player, .opponent = board.opponent};
				do_move(&new_board, i);

				// We want the perspective of the other player in the recursive call
				switch_boards(&new_board);

#ifdef PARALLEL
#pragma omp parallel
				for (uint8_t depth_delta = 0; !finished && depth_delta < depth_inc; depth_delta++)
					negamax(new_board, depth + depth_delta, -INFINITY, INFINITY, 1);
#else
				negamax(new_board, depth, -INFINITY, INFINITY, 1);
#endif
			}
		}

#ifdef METRICS
		levels_evaluated += depth;
		nr_moves++;
#endif
	}

	// Retrieve the best move from the hashtable
	int8_t best_move = get_best_move(board, valid);

	free_map();

	return best_move;
}

void print_ai_metrics(void) {
#ifdef METRICS
	printf("AI:\n");
	printf("    Start Depth: %" PRIu8 "\n", START_DEPTH);
	printf("    Average Reached Depth: %" PRIu64 "\n", levels_evaluated / nr_moves);
	printf("    Nodes/s: %f\n", (double) nodes / ((double) nr_moves * (time_limit / 1000.0)));
	printf("    Branches: %" PRIu64 "\n", branches);
	printf("    Branches explored: %" PRIu64 "\n", branches_evaluated);
	printf("    Branches pruned: %" PRIu64 "\n", branches - branches_evaluated);
	printf("    Branch factor: %f\n", (double) branches / (double) nodes);
	printf("    %% Pruned: %f\n", 100.0 * ((double) branches - (double) branches_evaluated) / branches);
	printf("    Nodes considered: %" PRIu64 "\n", nodes);
	printf("    Nodes evaluated: %" PRIu64 "\n", nodes_evaluated);
	printf("    Unique nodes evaluated: %" PRIu64 "\n", unique_nodes);
	printf("    %% Unique nodes : %f\n", 100 * (double) unique_nodes / (double) nodes_evaluated);
#endif
}
