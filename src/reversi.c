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
coordinate_t* possible_moves;
/**
 * Number of moves that are valid in the current boardposition
 */
uint8_t nr_possible_moves;

/**
 * Stores the intermediate result of all pieces that must be flipped when a
 * move is performed
 */
uint64_t to_flip[8][8] = {{0}};

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

coordinate_t human_turn(void) {
	coordinate_t choice;

	print_state(board, valid_moves, true);
	printf("Coordinate? ");

	// Read a-h, convert to 0-7
	choice.column = (uint8_t) getchar() - 97;
	choice.row = (uint8_t) getchar() - 49;
	// Skip over \n
	getchar();

	debug_print("Coordinate entered. Column: %c Row: %" PRIu8 "\n", choice.column + 97, choice.row + 1);

	if (choice.column > 7 || choice.row > 7) {
		printf("ERROR: Invallid coordinate entered. Column: %c Row: %" PRIu8 "\n", choice.column + 97, choice.row + 1);
		exit(EXIT_FAILURE);
	}

	if (!is_piece(valid_moves, choice.column, choice.row)) {
		printf("ERROR: Invalid location for piece: %c%" PRIu8 "\n", choice.column + 97, choice.row + 1);
		exit(EXIT_FAILURE);
	}

	return choice;
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

	coordinate_t choice;

	if ((turn == White && white == Human) || (turn == Black && black == Human)) {
		choice = human_turn();
	} else {
		uint8_t best_move = ai_turn(board);
		// TODO: See issue #10
		choice.column = best_move % 8;
		choice.row = best_move / 8;
	}

	do_move(&board, choice.column, choice.row, to_flip);
}

int main(void) {
	// Print settings for unicode characters
	setlocale(LC_CTYPE, "");

	// Setup game
	to_flip[2][3] = 0b0000000000000000000000000001000000000000000000000000000000000000;
	to_flip[3][2] = 0b0000000000000000000000000001000000000000000000000000000000000000;
	to_flip[4][5] = 0b0000000000000000000000000000000000001000000000000000000000000000;
	to_flip[5][4] = 0b0000000000000000000000000000000000001000000000000000000000000000;
	possible_moves = malloc(POSSIBLE_MOVES_MAX * sizeof(coordinate_t));
	possible_moves[0].column = 3;
	possible_moves[0].row = 2;
	possible_moves[1].column = 5;
	possible_moves[1].row = 4;
	possible_moves[2].column = 4;
	possible_moves[2].row = 5;
	possible_moves[3].column = 2;
	possible_moves[3].row = 3;
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
