#include <inttypes.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "state.h"

#define DEBUG 1
#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

void perform_turn(void) {
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

	//TODO: Check for validity of move
	// Read a-h, convert to 0-7
	uint8_t column = (uint8_t) getchar() - 97;
	uint8_t row = (uint8_t) getchar() - 49;
	// Skip over \n
	getchar();

	debug_print("Coordinate entered. Column: %c Row: %" PRIu8 "\n", column + 97, row + 1);

	if (column > 7 || row > 7) {
		printf("ERROR: Invallid coordinate entered. Column: %c Row: %" PRIu8 "\n", column + 97, row + 1);
		exit(EXIT_FAILURE);
	}

	if (state.valid_moves[column][row]) {
		place_piece(column, row, state.turn);
	} else {
		printf("ERROR: Invalid location for piece: %c%" PRIu8 "\n", column + 97, row + 1);
		exit(EXIT_FAILURE);
	}

	state.turn = state.turn == White ? Black : White;
}

int main(void) {
	// Print settings for unicode characters
	setlocale(LC_CTYPE, "");

	// Setup game
	state = default_state();

	print_state();

	while (any_move_valid()) {
		perform_turn();
		update_valid_moves();
		print_state();
	}

	return 1;
}
