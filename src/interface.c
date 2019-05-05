#include <inttypes.h>
#include <locale.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

#include "ai.hpp"
#include "debug.hpp"
#include "state_t.hpp"

typedef enum {
	HUMAN, AI
} player_t;

typedef enum {
	BLACK, WHITE
} color_t;

board_t board;
color_t turn;

player_t white = AI;
player_t black = HUMAN;

bool white_skipped;
bool black_skipped;
bool finished;

void setup(void) {
	turn = BLACK;

	white_skipped = false;
	black_skipped = false;

	board.player = 0b0000000000000000000000000000100000010000000000000000000000000000;
	board.opponent = 0b0000000000000000000000000001000000001000000000000000000000000000;
}

void switch_players(void) {
	switch_boards(&board);
	turn = (turn == WHITE) ? BLACK : WHITE;
}

uint8_t to_coordinate(uint8_t x, uint8_t y) {
	uint8_t x_p = 7 - (x - 97);
	uint8_t y_p = 7 - (y - 49);

	if (x_p > 7 || y_p > 7) {
		printf("ERROR: Invalid coordinate entered. Column: %" PRIu8 " Row: %" PRIu8 "\n", x_p, y_p);
		exit(EXIT_FAILURE);
	}

	return x_p + y_p * 8;
}

void from_coordinate(uint8_t c, char *column, char *row) {
	static char letters[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
	static char numbers[8] = {'1', '2', '3', '4', '5', '6', '7', '8'};
	c = 63 - c;
	*column = letters[c % 8];
	*row = numbers[c / 8];
}

uint8_t human_turn(void) {
	uint64_t valid_moves = get_valid_moves(board);

	print_state(board, valid_moves, true);
	printf("Coordinate? ");

	// Read a-h, convert to 0-7
	uint8_t choice_column = (uint8_t) getchar();
	uint8_t choice_row = (uint8_t) getchar();
	// Skip over \n
	getchar();

	debug_print("Coordinate entered. Column: %c Row: %" PRIu8 "\n", choice_column, choice_row);

	uint8_t coordinate = to_coordinate(choice_column, choice_row);

	if (!is_set(valid_moves, coordinate)) {
		printf("ERROR: Invalid location for piece: %c%" PRIu8 "\n", choice_column + 97, choice_row + 1);
		exit(EXIT_FAILURE);
	}

	return coordinate;
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

	uint8_t choice;
	switch (turn == WHITE ? white : black) {
		case HUMAN:
			choice = human_turn();
			break;
		case AI:
			choice = ai_turn(board);
			char c, r;
			from_coordinate(choice, &c, &r);
			printf("AI choose: %c%c\n", c, r);
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

	uint8_t opponent_score = 0;
	uint8_t player_score = 0;

	setup();
	play();
	if (finished) {
		player_score = (turn == BLACK) ? count(board.player) : count(board.opponent);
		opponent_score = (turn == BLACK) ? count(board.opponent) : count(board.player);

		if (opponent_score > player_score)
			printf("You won\n");
		else if (player_score > opponent_score)
			printf("The AI won\n");
		else
			printf("No one won\n");
	}

	return 0;
}
