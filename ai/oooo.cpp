#include <iostream>
#include <sstream>
#include <string>

#include "ai.hpp"
#include "../lib/state_t.hpp"

static board_t board;
static bool start_player = true;

static void debug(void) {
	if (start_player) {
		print_state(board, 0, false);
	} else {
		switch_boards(&board);
		print_state(board, 0, false);
		switch_boards(&board);
	}
}

static void go(void) {
	uint64_t time_ms = 10000;

	std::string arg;
	std::string line;
	std::getline(std::cin, line);
	std::istringstream iss(line);
	while (iss >> arg) {
		if (arg == "time")
			iss >> time_ms;
		else
			std::cerr << "Unrecognized sub-command: " << arg << std::endl;
	}

	int8_t choice = ai_turn(board, time_ms);
	char c, r;
	from_coordinate(choice, &c, &r);
	std::cout << "bestmove " << c << r << std::endl;

	do_move(&board, choice);
	switch_boards(&board);
	start_player = !start_player;
	if (!has_valid_move(board)) {
		switch_boards(&board);
		start_player = !start_player;
	}
}

static void play(void) {
	char column, row;

	std::cin >> column >> row;

	uint8_t choice_column = 7 - ((uint8_t) column - 97);
	uint8_t choice_row = 7 - ((uint8_t) row - 49);

	uint8_t coordinate = choice_column + choice_row * 8;

	do_move(&board, coordinate);
	switch_boards(&board);
	start_player = !start_player;
}

static void set_pos(void) {
	std::string command;

	std::cin >> command;

	if (command == "startpos") {
		board.player = 0b0000000000000000000000000000100000010000000000000000000000000000;
		board.opponent = 0b0000000000000000000000000001000000001000000000000000000000000000;
	} else if (command == "bitboard") {
		std::cin >> board.player >> board.opponent;
	} else {
		std::cerr << "Unrecognized sub-command: " << command << std::endl;
	}
}

static void set_var(void) {
	bool got_name = false;
	bool got_value = false;

	std::string command;
	std::string name;
	std::string value;

	 do {
		std::cin >> command;

		if (command == "name") {
			got_name = true;
			std::cin >> name;
		} else if (command == "value") {
			got_value = true;
			std::cin >> value;
		} else {
			std::cerr << "Unrecognized sub-command: " << command << std::endl;
			return;
		}
	} while (!got_name || !got_value);

	if (name == "MaxDepth") {
		set_max_depth((uint8_t) std::stoi(value));
	}
}

int main(void) {
	bool finished = false;
	std::string command;

	while (!finished) {
		std::cin >> command;

		if (command == "debug")
			debug();
		else if (command == "go")
			go();
		else if (command == "play")
			play();
		else if (command == "position")
			set_pos();
		else if (command == "setoption")
			set_var();
		else if (command == "quit")
			finished = true;
		else
			std::cerr << "Unrecognized command: " << command << std::endl;
	}

	return 0;
}
