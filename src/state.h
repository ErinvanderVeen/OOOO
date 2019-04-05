#ifndef STATE_H
#define STATE_H

#include "debug.h"

/**
 * Bitboard of all pieces considered to belong to the current player.
 * Note, there is no explicit distinction between white/black, as there
 * is no need for the engine to distinguish between if it is black or white.
 * All such information should be in the wrapper.
 */
uint64_t player_b;

/**
 * Bitboard of all pieces considered to belong to the opposing player
 */
uint64_t opponent_b;

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
uint64_t to_flip[8][8] = {{0}};

/**
 * Initializes the default state of Othello
 */
void init_state(void) {
	player_b = 0b0000000000000000000000000000100000010000000000000000000000000000;
	opponent_b = 0b0000000000000000000000000001000000001000000000000000000000000000;
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
	uint64_t temp = player_b;
	player_b = opponent_b;
	opponent_b = temp;
}

/**
 * Checks if the specified board has a piece in the specified location
 *
 * @param[in] The column of the desired location
 * @param[in] The column of the desired location
 * @param[in] The row of the desired location
 */
bool is_piece(uint64_t board, uint8_t column, uint8_t row) {
	// TODO: Possible optimization by mirroring the board?
	uint8_t column_mask = (uint64_t) 1 << (7 - column);
	uint8_t column_val = board >> ((7 - row) * 8);
	return (column_mask & column_val) > 0;
}

/**
 * Places a piece on the specified location of the specified board
 *
 * @param[in,out] The board on which the piece must be placed
 * @param[in] The column of the desired location
 * @param[in] The row of the desired location
 */
void place_piece(uint64_t *board, uint8_t column, uint8_t row) {
	uint64_t mask = (uint64_t) 1 << (((7 - row) * 8) + (7 - column));
	*board |= mask;
}

/**
 * Removes the piece of the board on the specified location
 *
 * @param[in,out] The board of which the piece must be removed
 * @param[in] The column of the desired location
 * @param[in] The row of the desired location
 */
void remove_piece(uint64_t* board, uint8_t column, uint8_t row) {
	uint64_t mask = (uint64_t) 1 << (((7 - row) * 8) + (7 - column));
	mask ^= UINT64_MAX;
	*board &= mask;
}

/**
 * Sets a value of the valid_move bitboard.
 *
 * @param[out] The bitboard with the valid moves
 * @param[in] The column of the desired location
 * @param[in] The row of the desired location
 * @param[in] Wether the move would be valid/invalid.
 */
void set_valid_move(uint64_t *valid_moves, uint8_t column, uint8_t row, bool valid) {
	uint64_t mask = (uint64_t) 1 << (((7 - row) * 8) + (7 - column));
	if (valid) {
		*valid_moves |= mask;
	} else {
		mask ^= UINT64_MAX;
		*valid_moves &= mask;
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
		if (is_piece(player_b, x, y)) {
			printf("\u26AA");
		} else if (is_piece(opponent_b, x, y)) {
			printf("\u26AB");
		} else if (is_piece(valid_moves, x, y) && show_valid_moves) {
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

/**
 * Flips pieces on the board given the instruction on what pieces to flip
 *
 * @param[in,out] The player board
 * @param[in,out] The opponents board
 * @param[in] The mask of the pieces to be flipped
 */
void flip_neighbours(uint64_t *player_b, uint64_t *opponent_b, uint64_t flip_mask) {
	*player_b |= flip_mask;
	*opponent_b &= *player_b ^ UINT64_MAX;
}

/**
 * Place a piece on the field. First performs check to see if the field is not
 * already occupied.
 *
 * @param[in] The column of the desired location
 * @param[in] The row of the desired location
 */
void do_move(uint8_t column, uint8_t row) {
	if (is_piece(player_b | opponent_b, column, row)) {
		printf("ERROR: Tried to place piece on occupied field\n");
		exit(EXIT_FAILURE);
	}

	debug_print("Placing piece at: %c%" PRIu8 "\n", column + 97, row + 1);

	place_piece(&player_b, column, row);

	flip_neighbours(&player_b, &opponent_b, to_flip[column][row]);
}

/**
 * Check if a move is valid. As long as there is an opponent piece in the
 * direction currently looked at, we will move one further step in the same
 * direction until either the edge or our own piece is encountered.
 *
 * @param[in] The column of the desired location
 * @param[in] The row of the desired location
 */
bool valid_any(uint8_t column, uint8_t row) {
	bool is_valid = false;
	to_flip[column][row] = 0;

	for(int8_t y = -1; y <= 1; y++) {
		for(int8_t x = -1; x <= 1; x++) {
			int8_t xx = column + x;
			int8_t yy = row + y;
			if((x == 0 && y == 0) || !is_piece(opponent_b, xx, yy))
				continue;

			do {
				xx += x;
				yy += y;
				if (!is_piece(opponent_b, xx, yy))
					break;
			} while (xx < 8 && xx >= 0 && yy < 8 && yy >= 0);

			if (is_piece(player_b, xx, yy)) {
				for (; xx != column || yy != row; xx -= x, yy -= y) {
					to_flip[column][row] |= (uint64_t) 1 << ((7 - xx) + ((7 - yy) * 8));
				}
				is_valid = true;
			}
		}
	}
	return is_valid;
}

/**
 * Check if the move is a valid move. NOTE: Does not perform a lookup in the
 * table, but calculates the value itself. Should be used to update the table.
 *
 * @param[in] The column of the desired location
 * @param[in] The row of the desired location
 */
bool is_valid_move(uint8_t column, uint8_t row) {
	if (is_piece(player_b | opponent_b, column, row))
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
			set_valid_move(&valid_moves, column, row, valid);
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
			if (is_piece(valid_moves, column, row))
				return true;
		}
	}
	return false;
}

#endif
