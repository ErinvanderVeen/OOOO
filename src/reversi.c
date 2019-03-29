#include <inttypes.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "state.h"
#include "ai.h"

player_t white = Human;
player_t black = Computer;

#define DEBUG 0
#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

bool finished = false;
bool white_skipped = false;
bool black_skipped = false;

coordinate_t human_turn(void) {
	coordinate_t choice;

	switch (state.turn) {
		case White:
			printf("White? ");
			break;
		case Black:
			printf("Black? ");
			break;
		default:
			printf("ERROR: Could not determine which player's turn it is.\n");
			exit(EXIT_FAILURE);
			break;
	}

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

	if (!state.valid_moves[choice.column][choice.row]) {
		printf("ERROR: Invalid location for piece: %c%" PRIu8 "\n", choice.column + 97, choice.row + 1);
		exit(EXIT_FAILURE);
	}

	return choice;
}

void perform_turn(void) {
	if (!any_move_valid()) {
		if (state.turn == White)
			white_skipped = true;
		else
			black_skipped = true;
		finished = white_skipped && black_skipped;
		return;
	}

	if (state.turn == White)
		white_skipped = false;
	else
		black_skipped = false;

	coordinate_t choice;

	if ((state.turn == White && white == Human) || (state.turn == Black && black == Human)) {
		choice = human_turn();
	} else {
		choice = ai_turn();
	}

	place_piece(choice.column, choice.row, state.turn);
}

int main(void) {
	// Print settings for unicode characters
	setlocale(LC_CTYPE, "");

	// Setup game
	init_state();

	// For the AI
	srand(time(NULL));

	while (!finished) {
		print_state(true);
		perform_turn();
		switch_player();
		update_valid_moves();
	}

	print_state(false);
	printf("Game ended, no valid moves.\n");

	return 1;
}
