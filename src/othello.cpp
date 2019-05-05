#include <inttypes.h>
#include <locale.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "ai.hpp"
#include "debug.hpp"
#include "state_t.hpp"

#define TIME_LIMIT 300

typedef enum {
	HUMAN, AI, RANDOM
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

uint8_t transcript_cnt;
uint8_t transcript[60];

void setup(void) {
	turn = BLACK;

	finished = false;
	white_skipped = false;
	black_skipped = false;

	board.player = 0b0000000000000000000000000000100000010000000000000000000000000000;
	board.opponent = 0b0000000000000000000000000001000000001000000000000000000000000000;

	transcript_cnt = 0;
	memset(transcript, 255, 60 * sizeof(char));
}

void switch_players(void) {
	switch_boards(&board);
	turn = (turn == WHITE) ? BLACK : WHITE;
}

void print_transcript(void) {
	printf("Transcript: ");
	for (int i = 0; i < 60; ++i) {
		if (transcript[i] == 255)
			break;

		char column, row;
		from_coordinate(transcript[i], &column, &row);
		printf("%c%c", column, row);
	}
	printf("\n");
}

uint8_t random_turn(void) {
	uint8_t choice;
	uint64_t valid = get_valid_moves(board);
	while (!is_set(valid, (choice = (uint8_t) rand() % 64)));
	return choice;
}

uint8_t human_turn(void) {
	uint64_t valid_moves = get_valid_moves(board);

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

	uint8_t choice = 0;
	switch (turn == WHITE ? white : black) {
		case HUMAN:
			choice = human_turn();
			break;
		case AI:
			choice = ai_turn(board);
			char c, r;
			from_coordinate(choice, &c, &r);
			printf("AI Chose: %c%c\n", c, r);
			break;
		case RANDOM:
			choice = random_turn();
			break;
	}

	transcript[transcript_cnt++] = choice;

	do_move(&board, choice);
}

void play(void) {
	while (!finished) {
		if (turn == WHITE) {
			uint64_t valid = get_valid_moves(board);

			// Must temporary switch boards in order to get the correct color when printing on each turn
			switch_boards(&board);
			print_state(board, valid, true);
			switch_boards(&board);
		} else {
			print_state(board, get_valid_moves(board), true);
		}
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
		print_transcript();
	}

	printf("Games/s:\t%.2f\n", (double) (win + loss + draw) / TIME_LIMIT);
	printf("AI wins:\t%.2f%%\n", (((double) win) / (win + loss + draw)) * 100);
	printf("Draws:\t%d\n", draw);

	return 0;
}
