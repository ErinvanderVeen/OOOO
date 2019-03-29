#ifndef STATE_H
#define STATE_H

#include "debug.h"

// Board state
typedef struct {
	// Two bitboards to represent the state of the board
	uint64_t player_pieces;
	uint64_t opponent_pieces;
	// Bit mask with valid moves
	uint64_t valid_moves;
} state_t;

state_t state;

// Used only for AI player
typedef struct {
	uint8_t column;
	uint8_t row;
} coordinate_t;

coordinate_t* possible_moves;
uint8_t nr_possible_moves;

void init_state(void) {
	// Inefficient TODO: Replace with more efficient alternative.
	state_t temp = {
		.player_pieces = 0b0000000000000000000000000000100000010000000000000000000000000000,
		.opponent_pieces = 0b0000000000000000000000000001000000001000000000000000000000000000,
		.valid_moves = 0b0000000000000000000100000010000000000100000010000000000000000000,
	};
	state = temp;
	nr_possible_moves = 4;
	// If not enough, extend
	possible_moves = malloc(20 * sizeof(coordinate_t));
	possible_moves[0].column = 3;
	possible_moves[0].row = 2;
	possible_moves[1].column = 5;
	possible_moves[1].row = 4;
	possible_moves[2].column = 4;
	possible_moves[2].row = 5;
	possible_moves[3].column = 2;
	possible_moves[3].row = 3;
	nr_possible_moves = 4;
}

// Switches the player and opponent pieces
void switch_players(void) {
	uint64_t temp = state.player_pieces;
	state.player_pieces = state.opponent_pieces;
	state.opponent_pieces = temp;
}

// Using bitmasks to determine if the location in the board is occupied by a
// piece of the current player
bool player_piece(uint8_t column, uint8_t row) {
	// TODO: Possible optimization by mirroring the board?
	uint8_t column_mask = (uint64_t) 1 << (7 - column);
	uint8_t column_val = state.player_pieces >> ((7 - row) * 8);
	return (column_mask & column_val) > 0;
}

// Using bitmasks to determine if the location in the board is occupied by a
// piece of the opposite player
bool opponent_piece(uint8_t column, uint8_t row) {
	// TODO: Possible optimization by mirroring the board?
	uint8_t column_mask = (uint64_t) 1 << (7 - column);
	uint8_t column_val = state.opponent_pieces >> ((7 - row) * 8);
	return (column_mask & column_val) > 0;
}

bool any_piece(uint8_t column, uint8_t row) {
	// TODO: Possible optimization by mirroring the board?
	// bitwise or increases efficiency of calling player_piece and opponent
	// piece seperately
	uint64_t board = state.player_pieces | state.opponent_pieces;
	uint8_t column_mask = (uint64_t) 1 << (7 - column);
	uint8_t column_val = board >> ((7 - row) * 8);
	return (column_mask & column_val) > 0;
}

// Places a piece at the specified location
void place_player_piece(uint8_t column, uint8_t row) {
	uint64_t mask = (uint64_t) 1 << (((7 - row) * 8) + (7 - column));
	state.player_pieces |= mask;
}

// Places a piece at the specified location
void remove_opponent_piece(uint8_t column, uint8_t row) {
	uint64_t mask = (uint64_t) 1 << (((7 - row) * 8) + (7 - column));
	mask ^= UINT64_MAX;
	state.opponent_pieces &= mask;
}

// Remove a piece at the specified location
void remove_player_piece(uint8_t column, uint8_t row) {
	uint64_t mask = (uint64_t) 1 << (((7 - row) * 8) + (7 - column));
	mask ^= UINT64_MAX;
	state.player_pieces &= mask;
}

// Remove a piece at the specified location
void place_opponent_piece(uint8_t column, uint8_t row) {
	uint64_t mask = (uint64_t) 1 << (((7 - row) * 8) + (7 - column));
	state.opponent_pieces |= mask;
}

bool valid_move(uint8_t column, uint8_t row) {
	// TODO: Possible optimization by mirroring the board?
	uint8_t column_mask = (uint64_t) 1 << (7 - column);
	uint8_t column_val = state.valid_moves >> ((7 - row) * 8);
	return (column_mask & column_val) > 0;
}

void set_valid_move(uint8_t column, uint8_t row, bool valid) {
	uint64_t mask = (uint64_t) 1 << (((7 - row) * 8) + (7 - column));
	if (valid) {
		state.valid_moves |= mask;
	} else {
		mask ^= UINT64_MAX;
		state.valid_moves &= mask;
	}
}

void print_line(uint8_t y, bool show_valid_moves) {
	printf("%" PRIu8 " ", y + 1);
	for (uint8_t x = 0; x < 8; x++) {
		printf("\u2502");
		if (player_piece(x,y)) {
			printf("\u26AA");
		} else if (opponent_piece(x,y)) {
			printf("\u26AB");
		} else if (valid_move(x, y) && show_valid_moves) {
			printf("\u25A1 ");
		} else {
			printf("  ");
		}
	}
	printf("\u2502\n");
}

void print_state(bool show_valid_moves) {
	// Duplicate horizontal bars because our pieces are double-width
	printf("\n  \u250C\u2500\u2500\u252C\u2500\u2500\u252C\u2500\u2500\u252C\u2500\u2500\u252C\u2500\u2500\u252C\u2500\u2500\u252C\u2500\u2500\u252C\u2500\u2500\u2510\n");
	print_line(0, show_valid_moves);
	for (uint8_t y = 1; y < 8; y++) {
		printf("  \u251C\u2500\u2500\u253C\u2500\u2500\u253C\u2500\u2500\u253C\u2500\u2500\u253C\u2500\u2500\u253C\u2500\u2500\u253C\u2500\u2500\u253C\u2500\u2500\u2524\n");
		print_line(y, show_valid_moves);
	}
	printf("  \u2514\u2500\u2500\u2534\u2500\u2500\u2534\u2500\u2500\u2534\u2500\u2500\u2534\u2500\u2500\u2534\u2500\u2500\u2534\u2500\u2500\u2534\u2500\u2500\u2518\n");
	printf("   a  b  c  d  e  f  g  h\n");
}

void flip_left(uint8_t column, uint8_t row) {
	do {
		column--;
	} while (opponent_piece(column, row) && column > 0);

	if (player_piece(column, row)) {
		column++;
		for(; opponent_piece(column, row); column++) {
			place_player_piece(column, row);
			remove_opponent_piece(column, row);
		}
	}
}

void flip_right(uint8_t column, uint8_t row) {
	do {
		column++;
	} while (opponent_piece(column, row) && column < 7);

	if (player_piece(column, row)) {
		column--;
		for(; opponent_piece(column, row); column--) {
			place_player_piece(column, row);
			remove_opponent_piece(column, row);
		}
	}
}

void flip_up(uint8_t column, uint8_t row) {
	do {
		row--;
	} while (opponent_piece(column, row) && row > 0);

	if (player_piece(column, row)) {
		row++;
		for(; opponent_piece(column, row); row++) {
			place_player_piece(column, row);
			remove_opponent_piece(column, row);
		}
	}
}

void flip_down(uint8_t column, uint8_t row) {
	do {
		row++;
	} while (opponent_piece(column, row) && row < 7);

	if (player_piece(column, row)) {
		row--;
		for(; opponent_piece(column, row); row--) {
			place_player_piece(column, row);
			remove_opponent_piece(column, row);
		}
	}
}

void flip_left_up(uint8_t column, uint8_t row) {
	do {
		column--;
		row--;
	} while (opponent_piece(column, row) && row > 0 && column > 0);

	if (player_piece(column, row)) {
		column++;
		row++;
		for(; opponent_piece(column, row); row++, column++) {
			place_player_piece(column, row);
			remove_opponent_piece(column, row);
		}
	}
}

void flip_right_up(uint8_t column, uint8_t row) {
	do {
		column++;
		row--;
	} while (opponent_piece(column, row) && row > 0 && column < 7);

	if (player_piece(column, row)) {
		column--;
		row++;
		for(; opponent_piece(column, row); row++, column--) {
			place_player_piece(column, row);
			remove_opponent_piece(column, row);
		}
	}
}

void flip_left_down(uint8_t column, uint8_t row) {
	do {
		column--;
		row++;
	} while (opponent_piece(column, row) && row < 7 && column > 0);

	if (player_piece(column, row)) {
		column++;
		row--;
		for(; opponent_piece(column, row); row--, column++) {
			place_player_piece(column, row);
			remove_opponent_piece(column, row);
		}
	}
}
void flip_right_down(uint8_t column, uint8_t row) {
	do {
		column++;
		row++;
	} while (opponent_piece(column, row) && row < 7 && column < 7);

	if (player_piece(column, row)) {
		column--;
		row--;
		for(; opponent_piece(column, row); row--, column--) {
			place_player_piece(column, row);
			remove_opponent_piece(column, row);
		}
	}
}

void flip_neighbours(uint8_t column, uint8_t row) {
	if (column != 0)
		flip_left(column, row);
	if (column != 7)
		flip_right(column, row);
	if (row != 0)
		flip_up(column, row);
	if (row != 7)
		flip_down(column, row);
	if (column != 0 && row != 0)
		flip_left_up(column, row);
	if (column != 7 && row != 0)
		flip_right_up(column, row);
	if (column != 0 && row != 7)
		flip_left_down(column, row);
	if (column != 7 && row != 7)
		flip_right_down(column, row);
}

void place_piece(uint8_t column, uint8_t row) {
	if (any_piece(column, row)) {
		printf("ERROR: Tried to place piece on occupied field\n");
		exit(EXIT_FAILURE);
	}

	debug_print("Placing piece at: %c%" PRIu8 "\n", column + 97, row + 1);

	place_player_piece(column, row);

	flip_neighbours(column, row);
}

bool valid_left(uint8_t column, uint8_t row) {
	if (column <= 1)
		return false;

	column--;

	if (opponent_piece(column, row)) {
		do {
			column--;
		} while (column > 0 && opponent_piece(column, row));

		if (player_piece(column, row))
			return true;
	}

	return false;
}

bool valid_right(uint8_t column, uint8_t row) {
	if (column >= 6)
		return false;

	column++;

	if (opponent_piece(column, row)) {
		do {
			column++;
		} while (column < 7 && opponent_piece(column, row));

		if (player_piece(column, row))
			return true;
	}

	return false;
}

bool valid_up(uint8_t column, uint8_t row) {
	if (row <= 1)
		return false;

	row--;

	if (opponent_piece(column, row)) {
		do {
			row--;
		} while (row > 0 && opponent_piece(column, row));

		if (player_piece(column, row))
			return true;
	}

	return false;
}

bool valid_down(uint8_t column, uint8_t row) {
	if (row >= 6)
		return false;

	row++;

	if (opponent_piece(column, row)) {
		do {
			row++;
		} while (row < 7 && opponent_piece(column, row));

		if (player_piece(column, row))
			return true;
	}

	return false;
}

bool valid_left_up(uint8_t column, uint8_t row) {
	if (column <= 1 || row <= 1)
		return false;

	column--;
	row--;

	if (opponent_piece(column, row)) {
		do {
			column--;
			row--;
		} while (column > 0 && row > 0 && opponent_piece(column, row));

		if (player_piece(column, row))
			return true;
	}

	return false;
}

bool valid_right_up(uint8_t column, uint8_t row) {
	if (column >= 6 || row <= 1)
		return false;

	column++;
	row--;

	if (opponent_piece(column, row)) {
		do {
			column++;
			row--;
		} while (column < 7 && row > 0&& opponent_piece(column, row));

		if (player_piece(column, row))
			return true;
	}

	return false;
}

bool valid_left_down(uint8_t column, uint8_t row) {
	if (column <= 1 || row >= 6)
		return false;

	column--;
	row++;

	if (opponent_piece(column, row)) {
		do {
			column--;
			row++;
		} while (column > 0 && row < 7 && opponent_piece(column, row));

		if (player_piece(column, row))
			return true;
	}

	return false;
}

bool valid_right_down(uint8_t column, uint8_t row) {
	if (column >= 6 || row >= 6)
		return false;

	column++;
	row++;

	if (opponent_piece(column, row)) {
		do {
			column++;
			row++;
		} while (column < 7 && row < 7 && opponent_piece(column, row));

		if (player_piece(column, row))
			return true;
	}

	return false;
}

bool is_valid_move(uint8_t column, uint8_t row) {
	if (any_piece(column, row))
		return false;

	return (
		valid_left(column, row) ||
		valid_right(column, row) ||
		valid_up(column, row) ||
		valid_down(column, row) ||
		valid_left_up(column, row) ||
		valid_right_up(column, row) ||
		valid_left_down(column, row) ||
		valid_right_down(column, row)
	);
}

void update_valid_moves(void) {
	nr_possible_moves = 0;
	for (uint8_t column = 0; column < 8; column++) {
		for (uint8_t row = 0; row < 8; row++) {
			bool valid = is_valid_move(column, row);
			set_valid_move(column, row, valid);
			if (valid) {
				possible_moves[nr_possible_moves].column = column;
				possible_moves[nr_possible_moves].row = row;
				nr_possible_moves++;
			}
		}
	}
}

// Checks a move is possible in any field
bool any_move_valid(void) {
	for (uint8_t column = 0; column <= 7; column++) {
		for (uint8_t row = 0; row <= 7; row++) {
			if (valid_move(column, row))
				return true;
		}
	}
	return false;
}

#endif
