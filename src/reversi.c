// Might not be enough, update
#define POSSIBLE_MOVES_MAX 16

#include <inttypes.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "state_t.h"
#include "ai.h"
#include "debug.h"

typedef enum {Human, Computer} player_t;

player_t white = Human;
player_t black = Computer;

typedef enum {Black, White} color_t;

/**
 * List of moves that are valid in the current boardposition
 */
uint8_t* possible_moves;
/**
 * Number of moves that are valid in the current boardposition
 */
uint8_t nr_possible_moves;

/**
 * Stores the intermediate result of all pieces that must be flipped when a
 * move is performed
 */
uint64_t to_flip[64] = {0};

color_t turn = Black;

bool finished = false;
bool white_skipped = false;
bool black_skipped = false;

/**
 * Bitboard of all pieces considered to belong to the current and opponent player.
 */
board_t board = {
	.player = 0b0000000000000000000000000000100000010000000000000000000000000000,
	.opponent = 0b0000000000000000000000000001000000001000000000000000000000000000
};

/**
 * Bitboard with valid moves
 */
uint64_t valid_moves = 0b0000000000000000000100000010000000000100000010000000000000000000;

/**
 * Swiches the current player with its opponent
 */
void switch_players(void) {
	uint64_t temp = board.player;
	board.player = board.opponent;
	board.opponent = temp;
}

uint8_t human_turn(void) {
	print_state(board, valid_moves, true);
	printf("Coordinate? ");

	// Read a-h, convert to 0-7
	uint8_t choice_column = 7 - ((uint8_t) getchar() - 97);
	uint8_t choice_row = 7 - ((uint8_t) getchar() - 49);
	// Skip over \n
	getchar();

	debug_print("Coordinate entered. Column: %c Row: %" PRIu8 "\n", choice_column + 97, choice_row + 1);

	if (choice_column > 7 || choice_row > 7) {
		printf("ERROR: Invallid coordinate entered. Column: %c Row: %" PRIu8 "\n", choice_column + 97, choice_row + 1);
		exit(EXIT_FAILURE);
	}

	uint8_t coordinate = choice_column + choice_row * 8;

	if (!is_piece(valid_moves, coordinate)) {
		printf("ERROR: Invalid location for piece: %c%" PRIu8 "\n", 7 - choice_column + 97, 7 - choice_row + 1);
		exit(EXIT_FAILURE);
	}

	return coordinate;
}

void perform_turn(void) {
	if (!any_move_valid(valid_moves)) {
		if (turn == White)
			white_skipped = true;
		else
			black_skipped = true;
		finished = white_skipped && black_skipped;
		return;
	}

	if (turn == White)
		white_skipped = false;
	else
		black_skipped = false;

	uint8_t choice;

	if ((turn == White && white == Human) || (turn == Black && black == Human)) {
		choice = human_turn();
	} else {
		choice = ai_turn(board);
	}

	do_move(&board, choice, to_flip);
}

int main(void) {
	// Print settings for unicode characters
	setlocale(LC_CTYPE, "");

	// Setup game
	to_flip[19] = 0b0000000000000000000000000000000000001000000000000000000000000000;
	to_flip[26] = 0b0000000000000000000000000000000000001000000000000000000000000000;
	to_flip[37] = 0b0000000000000000000000000001000000000000000000000000000000000000;
	to_flip[44] = 0b0000000000000000000000000001000000000000000000000000000000000000;
	possible_moves = malloc(POSSIBLE_MOVES_MAX * sizeof(possible_moves[0]));

	possible_moves[0] = 19;
	possible_moves[1] = 26;
	possible_moves[2] = 37;
	possible_moves[3] = 44;
	nr_possible_moves = 4;

	// For the AI
	srand(time(NULL));

	while (!finished) {
		perform_turn();
		switch_players();
		if (turn == White)
			turn = Black;
		else
			turn = White;
		update_valid_moves(board, &valid_moves, to_flip, possible_moves);
	}

	print_state(board, valid_moves, false);
	printf("Game ended, no valid moves.\n");

	return 1;
}
