#ifndef STATE_H
#define STATE_H

#include "debug.h"

/**
 * Bitboard of all pieces considered to belong to the current player.
 * Note, there is no explicit distinction between white/black, as there
 * is no need for the engine to distinguish between if it is black or white.
 * All such information should be in the wrapper.
 */
uint64_t player_pieces;

/**
 * Bitboard of all pieces considered to belong to the opposing player
 */
uint64_t opponent_pieces;

/**
 * Bitboard with valid moves
 */
uint64_t valid_moves;

/**
 * Used by the AI to send its desired move to the wrapper. Also used internally
 * to keep track of candidate moves.
 */
typedef struct {
	uint8_t column;
	uint8_t row;
} coordinate_t;

/**
 * List of moves that are valid in the current boardposition
 */
coordinate_t* possible_moves;
/**
 * Number of moves that are valid in the current boardposition
 */
uint8_t nr_possible_moves;

/**
 * Stores the intermediate result of all pieces that must be flipped when a
 * move is performed
 */
uint64_t to_flip[8][8] = {0};

/**
 * Initializes the default state of Othello
 */
void init_state(void) {
	player_pieces = 0b0000000000000000000000000000100000010000000000000000000000000000;
	opponent_pieces = 0b0000000000000000000000000001000000001000000000000000000000000000;
	valid_moves = 0b0000000000000000000100000010000000000100000010000000000000000000;

	to_flip[2][3] = 0b0000000000000000000000000001000000000000000000000000000000000000;
	to_flip[3][2] = 0b0000000000000000000000000001000000000000000000000000000000000000;
	to_flip[4][5] = 0b0000000000000000000000000000000000001000000000000000000000000000;
	to_flip[5][4] = 0b0000000000000000000000000000000000001000000000000000000000000000;

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

/**
 * Swiches the current player with its opponent
 */
void switch_players(void) {
	uint64_t temp = player_pieces;
	player_pieces = opponent_pieces;
	opponent_pieces = temp;
}

/**
 * Checks if the current player has a piece in the specified location
 *
 * @param[in] The column of the desired location
 * @param[in] The row of the desired location
 */
bool player_piece(uint8_t column, uint8_t row) {
	// TODO: Possible optimization by mirroring the board?
	uint8_t column_mask = (uint64_t) 1 << (7 - column);
	uint8_t column_val = player_pieces >> ((7 - row) * 8);
	return (column_mask & column_val) > 0;
}

/**
 * Checks if the opposing player has a piece in the specified location
 *
 * @param[in] The column of the desired location
 * @param[in] The row of the desired location
 */
bool opponent_piece(uint8_t column, uint8_t row) {
	// TODO: Possible optimization by mirroring the board?
	uint8_t column_mask = (uint64_t) 1 << (7 - column);
	uint8_t column_val = opponent_pieces >> ((7 - row) * 8);
	return (column_mask & column_val) > 0;
}

/**
 * Checks if the either player has a piece in the specified location
 *
 * @param[in] The column of the desired location
 * @param[in] The row of the desired location
 */
bool any_piece(uint8_t column, uint8_t row) {
	// TODO: Possible optimization by mirroring the board?
	// bitwise or increases efficiency of calling player_piece and opponent
	// piece seperately
	uint64_t board = player_pieces | opponent_pieces;
	uint8_t column_mask = (uint64_t) 1 << (7 - column);
	uint8_t column_val = board >> ((7 - row) * 8);
	return (column_mask & column_val) > 0;
}

/**
 * Places a piece of the current player on the specified location.
 * Note: Does not remove an opposing piece if it is already there.
 *
 * @param[in] The column of the desired location
 * @param[in] The row of the desired location
 */
void place_player_piece(uint8_t column, uint8_t row) {
	uint64_t mask = (uint64_t) 1 << (((7 - row) * 8) + (7 - column));
	player_pieces |= mask;
}

/**
 * Places a piece of the opposing player on the specified location.
 * Note: Does not remove an opposing piece if it is already there.
 * See switch_to_player_piece(uint8_t column, uint8_t row) if this is desired.
 * TODO: create switch_to_player
 *
 * @param[in] The column of the desired location
 * @param[in] The row of the desired location
 */
void place_opponent_piece(uint8_t column, uint8_t row) {
	uint64_t mask = (uint64_t) 1 << (((7 - row) * 8) + (7 - column));
	opponent_pieces |= mask;
}

/**
 * Removes the piece of the current player on the specified location.
 *
 * @param[in] The column of the desired location
 * @param[in] The row of the desired location
 */
void remove_player_piece(uint8_t column, uint8_t row) {
	uint64_t mask = (uint64_t) 1 << (((7 - row) * 8) + (7 - column));
	mask ^= UINT64_MAX;
	player_pieces &= mask;
}

/**
 * Removes the piece of the opposing player on the specified location.
 *
 * @param[in] The column of the desired location
 * @param[in] The row of the desired location
 */
void remove_opponent_piece(uint8_t column, uint8_t row) {
	uint64_t mask = (uint64_t) 1 << (((7 - row) * 8) + (7 - column));
	mask ^= UINT64_MAX;
	opponent_pieces &= mask;
}

/**
 * Looks in the valid_move bitboard if the specified location would be a valid
 * move for the current player.
 *
 * @param[in] The column of the desired location
 * @param[in] The row of the desired location
 */
bool valid_move(uint8_t column, uint8_t row) {
	// TODO: Possible optimization by mirroring the board?
	uint8_t column_mask = (uint64_t) 1 << (7 - column);
	uint8_t column_val = valid_moves >> ((7 - row) * 8);
	return (column_mask & column_val) > 0;
}

/**
 * Sets a value of the valid_move bitboard.
 *
 * @param[in] The column of the desired location
 * @param[in] The row of the desired location
 * @param[in] Wether the move would be valid/invalid.
 */
void set_valid_move(uint8_t column, uint8_t row, bool valid) {
	uint64_t mask = (uint64_t) 1 << (((7 - row) * 8) + (7 - column));
	if (valid) {
		valid_moves |= mask;
	} else {
		mask ^= UINT64_MAX;
		valid_moves &= mask;
	}
}


/**
 * Print a graphical representation of a row in the field.
 *
 * @param[in] The row
 * @param[in] Wether or not valid moves should be highlighted
 */
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

/**
 * Print a graphical representation of the entire field
 *
 * @param[in] Wether or not valid moves should be highlighted
 */
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

void flip_neighbours(uint8_t column, uint8_t row) {
	player_pieces |= to_flip[column][row];
	opponent_pieces &= player_pieces ^ UINT64_MAX;
}

/**
 * Place a piece on the field. First performs check to see if the field is not
 * already occupied.
 *
 * @param[in] The column of the desired location
 * @param[in] The row of the desired location
 */
void place_piece(uint8_t column, uint8_t row) {
	if (any_piece(column, row)) {
		printf("ERROR: Tried to place piece on occupied field\n");
		exit(EXIT_FAILURE);
	}

	debug_print("Placing piece at: %c%" PRIu8 "\n", column + 97, row + 1);

	place_player_piece(column, row);

	flip_neighbours(column, row);
}

/**
 * Check if a move is valid. As long as there is an opponent piece in the 
 * direction currently looked at, we will move one further step in the same
 * direction until either the edge or our own piece is encountered.
 *
 * @param[in] The column of the desired location
 * @param[in] The row of the desired location
 */
bool valid_any(uint8_t col, uint8_t row) {
	for(int8_t y = -1; y <= 1; y++) {
		for(int8_t x = -1; x <= 1; x++) {
			int8_t xx = col + x;
			int8_t yy = row + y;
			if(x == 0 && y == 0 || !opponent_piece(xx, yy))
				continue;
			do {
				xx += x;
				yy += y;
				if(!opponent_piece(xx, yy))
					break;
			} while(xx < 8 && xx >= 0 && yy < 8 && yy >= 0);
			
			if (player_piece(xx, yy))
				return true;
		}
	}
	return false;
}

/**
 * Check if the move is a valid move. NOTE: Does not perform a lookup in the
 * table, but calculates the value itself. Should be used to update the table.
 *
 * @param[in] The column of the desired location
 * @param[in] The row of the desired location
 */
bool is_valid_move(uint8_t column, uint8_t row) {
	if (any_piece(column, row))
		return false;

	return valid_any(column, row);
}

/**
 * Updates the valid_move bitboard. By checking for every square if it would
 * be a valid move.
 */
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

/**
 * Checks if the current player can perform ANY move.
 */
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
