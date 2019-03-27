#include <inttypes.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "state.h"

#define DEBUG 0
#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

bool finished = false;

void perform_turn(void) {
	if (!piece_move_valid()) {
		if (skip_move_valid()) {
			if (state.turn == White)
				state.white_skip = false;
			state.black_skip = false;
			return;
		} else {
			finished = true;
			return;
		}
	}

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
}

int main(void) {
	// Print settings for unicode characters
	setlocale(LC_CTYPE, "");

	// Setup game
	state = default_state();

	while (!finished) {
		print_state(false);
		perform_turn();
		switch_player();
		update_valid_moves();
	}

	print_state(false);
	printf("Game ended, no valid moves.\n");
	return 1;
}
