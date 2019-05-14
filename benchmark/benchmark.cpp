#include <inttypes.h>
#include <locale.h>
#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../ai/ai.hpp"
#include "../lib/debug.hpp"
#include "../lib/state_t.hpp"
#include "../lib/eval_hashmap.hpp"

#define TIME_LIMIT 60

typedef enum {
	AI, RANDOM
} player_t;

typedef enum {
	BLACK, WHITE
} color_t;

board_t board;
color_t turn;

player_t white = RANDOM;
player_t black = AI;

bool white_skipped;
bool black_skipped;
bool finished;

void setup(void) {
	turn = BLACK;

	finished = false;
	white_skipped = false;
	black_skipped = false;

	board.player = 0b0000000000000000000000000000100000010000000000000000000000000000;
	board.opponent = 0b0000000000000000000000000001000000001000000000000000000000000000;
}

void switch_players(void) {
	switch_boards(&board);
	turn = (turn == WHITE) ? BLACK : WHITE;
}

uint8_t random_turn(void) {
	uint8_t choice;
	uint64_t valid = get_valid_moves(board);
	while (!is_set(valid, (choice = (uint8_t) rand() % 64)));
	return choice;
}

void perform_turn(void) {
	if (!has_valid_move(board)) {
		if (turn == WHITE)
			white_skipped = true;
		else
			black_skipped = true;
		finished = white_skipped && black_skipped;
		return;
	}

	if (turn == WHITE)
		white_skipped = false;
	else
		black_skipped = false;

	uint8_t choice = 0;
	switch (turn == WHITE ? white : black) {
		case AI:
			choice = ai_turn(board, 100);
			break;
		case RANDOM:
			choice = random_turn();
			break;
	}

	do_move(&board, choice);
}

void play(void) {
	while (!finished) {
		perform_turn();
		switch_players();
	}
}

int main(void) {
	setlocale(LC_CTYPE, "");

	// Replace time(NULL) with a constant in order to get reproducible random AI moves
	srand(time(NULL));

	uint8_t win = 0;
	uint8_t loss = 0;
	uint8_t draw = 0;

	uint8_t opponent_score = 0;
	uint8_t player_score = 0;

	time_t last = time(NULL);
	while (time(NULL) - last < TIME_LIMIT) {
		setup();
		play();
		if (finished) {
			player_score = (turn == BLACK) ? count(board.player) : count(board.opponent);
			opponent_score = (turn == BLACK) ? count(board.opponent) : count(board.player);

			if (opponent_score > player_score)
				loss++;
			else if (player_score > opponent_score)
				win++;
			else
				draw++;
		}
	}

	printf("```\n");
#ifdef PARALLEL
	printf("Number of Threads: %d\n", omp_get_max_threads());
#endif
	printf("Games/s: %.2f\n", (double) (win + loss + draw) / TIME_LIMIT);
	printf("AI wins: %.2f%%\n", (((double) win) / (win + loss + draw)) * 100);
	printf("Draws: %d\n", draw);
	print_ai_metrics();
	print_hash_metrics();
	printf("```\n");

	return 0;
}
