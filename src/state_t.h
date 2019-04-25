#ifndef STATE_T_H
#define STATE_T_H

#define ONE (uint64_t) 1

#include "debug.h"

/**
 * Holds the most common way the board state is represented
 * Note the bottom right square is the least significant bit
 *   ┌──┬──┬──┬──┬──┬──┬──┬──┐
 * 1 │63│62│61│60│59│58│57│56│
 *   ├──┼──┼──┼──┼──┼──┼──┼──┤
 * 2 │55│54│53│52│51│50│49│48│
 *   ├──┼──┼──┼──┼──┼──┼──┼──┤
 * 3 │47│46│45│44│43│42│41│40│
 *   ├──┼──┼──┼──┼──┼──┼──┼──┤
 * 4 │39│38│37│36│35│34│33│32│
 *   ├──┼──┼──┼──┼──┼──┼──┼──┤
 * 5 │31│30│29│28│27│26│25│24│
 *   ├──┼──┼──┼──┼──┼──┼──┼──┤
 * 6 │23│22│21│20│19│18│17│16│
 *   ├──┼──┼──┼──┼──┼──┼──┼──┤
 * 7 │15│14│13│12│11│10│ 9│ 8│
 *   ├──┼──┼──┼──┼──┼──┼──┼──┤
 * 8 │ 7│ 6│ 5│ 4│ 3│ 2│ 1│ 0│
 *   └──┴──┴──┴──┴──┴──┴──┴──┘
 *    a  b  c  d  e  f  g  h
 */
typedef struct {
	uint64_t player;
	uint64_t opponent;
} board_t;

/**
 * Checks if the specified board has a piece in the specified location
 *
 * @param[in] Board that should be checked for a piece
 * @param[in] The coordinate of the desired location
 */
bool is_piece(uint64_t board, uint8_t coordinate) {
	uint64_t mask = ONE << coordinate;
	return (board & mask) > 0;
}

/**
 * Places a piece on the specified location of the specified board
 *
 * @param[in,out] Board that should be checked for a piece
 * @param[in] The coordinate of the desired location
 */
void place_piece(uint64_t *board, uint8_t coordinate) {
	uint64_t mask = ONE << coordinate;
	*board |= mask;
}

/**
 * Removes the piece of the board on the specified location
 *
 * @param[in,out] The board of which the piece must be removed
 * @param[in] The coordinate of the desired location
 */
void remove_piece(uint64_t* board, uint8_t coordinate) {
	uint64_t mask = ONE << coordinate;
	*board &= ~mask;
}

/**
 * Sets a value of the valid_move bitboard.
 *
 * @param[out] The bitboard with the valid moves
 * @param[in] The coordinate of the desired location
 * @param[in] Wether the move would be valid/invalid.
 */
void set_valid_move(uint64_t *valid_moves, uint8_t coordinate, bool valid) {
	uint64_t mask = ONE << coordinate;
	if (valid)
		*valid_moves |= mask;
	else
		*valid_moves &= ~mask;
}

/**
 * Print a graphical representation of the entire field
 *
 * @param[in] Wether or not valid moves should be highlighted
 */
void print_state(board_t board, uint64_t valid_moves, bool show_valid_moves) {
	// Duplicate horizontal bars because our pieces are double-width
	for (int8_t y = 63; y >= 0; y -= 8) {
		printf("  -----------------\n");
		printf("%" PRIu8 " ", (7 - (y / 8)) + 1);
		for (int8_t x = y; x >= y - 7; x--) {
			printf("|");
			if (is_piece(board.player, x)) {
				printf("P");
			} else if (is_piece(board.opponent, x)) {
				printf("o");
			} else if (is_piece(valid_moves, x) && show_valid_moves) {
				printf("*");
			} else {
				printf(" ");
			}
		}
		printf("|\n");
	}
	printf("   a b c d e f g h\n");
}

/**
 * Flips pieces on the board given the instruction on what pieces to flip
 *
 * @param[in,out] The player board
 * @param[in,out] The opponents board
 * @param[in] The mask of the pieces to be flipped
 */
void flip_neighbours(board_t *board, uint64_t flip_mask) {
	board->player |= flip_mask;
	board->opponent &= ~board->player;
}

/**
 * Place a piece on the field. First performs check to see if the field is not
 * already occupied.
 *
 * @param[in,out] The board on which the move should be made
 * @param[in] The coordinate of the desired location
 * @param[in] What should happen when the move is made
 */
void do_move(board_t *board, uint8_t coordinate, uint64_t to_flip[64]) {
	debug_print("Placing piece at: %" PRIu8 "\n", coordinate);

	if (is_piece(board->player | board->opponent, coordinate)) {
		printf("ERROR: Tried to place piece on occupied field\n");
		exit(EXIT_FAILURE);
	}

	place_piece(&board->player, coordinate);

	flip_neighbours(board, to_flip[coordinate]);
}

/**
 * Check if the move is a valid move. NOTE: Does not perform a lookup in the
 * table, but calculates the value itself. Should be used to update the table.
 *
 * @param[in] The board on which we should check the validity
 * @param[in] The coordinate of the desired location
 * @param[out] Contains the pieces that should be flipped when this move turns out to be valid
 */
bool is_valid_move(board_t board, uint8_t coordinate, uint64_t to_flip[64]) {
	uint8_t row = coordinate / 8;
	uint8_t column = coordinate % 8;
	if (is_piece(board.player | board.opponent, coordinate))
		return false;

	bool is_valid = false;
	to_flip[coordinate] = 0;
	static int8_t offsets[8][2] = {{0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}};

	for(uint8_t i = 0; i < 8; i++) {
		int8_t x = column + offsets[i][0];
		int8_t y = row + offsets[i][1];
		if(!is_piece(board.opponent, y * 8 + x))
			continue;

		while (x < 8 && x >= 0 && y < 8 && y >= 0){
			x += offsets[i][0];
			y += offsets[i][1];
			if (!is_piece(board.opponent,  y * 8 + x))
				break;
		} 

		if (is_piece(board.player, y * 8 + x)) {
			for (; x != column || y != row; x -= offsets[i][0], y -= offsets[i][1]) {
				to_flip[coordinate] |= (uint64_t) 1 << ((7 - x) + ((7 - y) * 8));
			}
			is_valid = true;
		}
	}
	return is_valid;
}

/**
 * Updates the valid_move bitboard. By checking for every square if it would
 * be a valid move.
 */
void update_valid_moves(board_t board, uint64_t *valid_moves, uint64_t to_flip[64], uint8_t possible_moves[POSSIBLE_MOVES_MAX]) {
	uint8_t nr_possible_moves = 0;
	for (uint8_t coordinate = 0; coordinate < 64; coordinate++) {
		bool valid = is_valid_move(board, coordinate, to_flip);
		set_valid_move(valid_moves, coordinate, valid);
		if (valid) {
			possible_moves[nr_possible_moves] = coordinate;
			nr_possible_moves++;
		}
	}
	// Indicates the end of the array without having the specify the size
	possible_moves[nr_possible_moves] = 64;
}

/**
 * Checks if the current player can perform ANY move.
 */
bool any_move_valid(uint64_t valid_moves) {
	return valid_moves > 0;
}

#endif
