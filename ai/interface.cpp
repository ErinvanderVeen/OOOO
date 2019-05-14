#include <iostream>
#include <sstream>
#include <string>

#include "ai.hpp"
#include "../lib/state_t.hpp"

board_t board;

static void go(void) {
	uint64_t time_ms = 1000;

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

		if (command == "go")
			go();
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
