#include <inttypes.h>
#include <iostream>
#include <locale.h>
#include <pstreams/pstream.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <time.h>

#include "../lib/debug.hpp"
#include "../lib/state_t.hpp"

typedef enum {
	HUMAN, AI
} player_t;

typedef enum {
	BLACK, WHITE
} color_t;

board_t board;
color_t turn;

redi::pstream black_engine;
redi::pstream white_engine;

player_t black;
player_t white;

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

	if (black == AI)
		black_engine << "position startpos" << std::endl;
	if (white == AI)
		white_engine << "position startpos" << std::endl;
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

uint8_t human_turn(void) {
	uint64_t valid_moves = get_valid_moves(board);

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
		printf("ERROR: Invalid location for piece: %c%" PRIu8 "\n", 7 - (choice_column + 97), 7 - (choice_row + 1));
		exit(EXIT_FAILURE);
	}

	return coordinate;
}

uint8_t ai_turn(void) {
	char c, r;
	std::string command;
	std::cout << "AI is thinking..." << std::endl;
	redi::pstream engine;
	switch (turn) {
		case BLACK:
			black_engine << "go time 1000" << std::endl;
			do {
				black_engine >> command;
				std::cout << command << " ";
			} while (command != "bestmove");
			black_engine >> c >> r;
			break;
		case WHITE:
			white_engine << "go time 1000" << std::endl;
			do {
				white_engine >> command;
				std::cout << command << " ";
			} while (command != "bestmove");
			white_engine >> c >> r;
			break;
	}

	std::cout << c << r;
	std::cout << std::endl;

	uint8_t choice_column = 7 - ((uint8_t) c - 97);
	uint8_t choice_row = 7 - ((uint8_t) r - 49);
	uint8_t choice = choice_column + choice_row * 8;

	return choice;
}

void notify_engines(char c, char r) {
	if (turn == BLACK && white == AI)
		white_engine << "play " << c << r << std::endl;

	if (turn == WHITE && black == AI)
		black_engine << "play " << c << r << std::endl;
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
			choice = ai_turn();
			break;
		default:
			exit(EXIT_FAILURE);
	}

	do_move(&board, choice);
	char c, r;
	from_coordinate(choice, &c, &r);
	notify_engines(c, r);
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

int main(int argc, char *argv[]) {
	if (argc != 3) {
		std::cout << "Usage: ./interface.out <black_player> <white_player>" << std::endl
		          << "Where:" << std::endl
		          << "\t<black_player> = HUMAN | /path/to/engine" << std::endl
		          << "\t<white_player> = HUMAN | /path/to/engine" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (strcmp(argv[1], "HUMAN") == 0) {
		black = HUMAN;
	} else {
		black = AI;
		const std::string path(argv[1]);
		black_engine.open(path, redi::pstreams::pstdout | redi::pstreams::pstdin);
	}

	if (strcmp(argv[2], "HUMAN") == 0) {
		white = HUMAN;
	} else {
		white = AI;
		const std::string path(argv[2]);
		white_engine.open(path, redi::pstreams::pstdout | redi::pstreams::pstdin);
	}

	setup();
	play();

	return 0;
}
