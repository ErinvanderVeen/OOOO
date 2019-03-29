#include <inttypes.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "state.h"
#include "ai.h"
#include "debug.h"

typedef enum {Human, Computer} player_t;

player_t white = Human;
player_t black = Computer;

typedef enum {Black, White} color_t;

color_t turn = Black;

bool finished = false;
bool white_skipped = false;
bool black_skipped = false;

coordinate_t human_turn(void) {
	coordinate_t choice;

	print_state(true);
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

	if (!valid_move(choice.column, choice.row)) {
		printf("ERROR: Invalid location for piece: %c%" PRIu8 "\n", choice.column + 97, choice.row + 1);
		exit(EXIT_FAILURE);
	}

	return choice;
}

void perform_turn(void) {
	if (!any_move_valid()) {
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
		choice = ai_turn();
	}

	place_piece(choice.column, choice.row);
}

int main(void) {
	// Print settings for unicode characters
	setlocale(LC_CTYPE, "");

	// Setup game
	init_state();

	// For the AI
	srand(time(NULL));

	while (!finished) {
		perform_turn();
		switch_players();
		if (turn == White)
			turn = Black;
		else
			turn = White;
		update_valid_moves();
	}

	print_state(false);
	printf("Game ended, no valid moves.\n");

	return 1;
}
