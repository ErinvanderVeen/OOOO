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

typedef enum {
	Up = 0,
	UpRight = 1,
	Right = 2,
	DownRight = 3,
	Down = 4,
	DownLeft = 5,
	Left = 6,
	UpLeft= 7
} direction_t;

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
 * Counts the number of pieces on the specified board
 */
uint8_t count_pieces(uint64_t board) {
	return __builtin_popcountll(board);
}

/**
 * Shift board in a certain direction
 *
 * @param The board
 * @param The direction
 *
 * Based on https://www.hanshq.net/othello.html however, with some things
 * changed. Including one bug-fix.
 */
static uint64_t shift(uint64_t board, direction_t direction) {
	// The direction refers to the direction we shift in. Not to the amount
	// of places that are supposed to be shifted.
	// Clockwise
	// 0: Up
	// 1: Up-right
	// 2: Right
	// 3: Down-right
	// 4: Down
	// 5: Down-left
	// 6: Left
	// 7: Up-left
	static const uint64_t MASKS[] = {
		0xFFFFFFFFFFFFFFFFULL, /* Up. */
		0x7F7F7F7F7F7F7F00ULL, /* Up-right. */
		0x7F7F7F7F7F7F7F7FULL, /* Right. */
		0x007F7F7F7F7F7F7FULL, /* Down-right. */
		0xFFFFFFFFFFFFFFFFULL, /* Down. */
		0x00FEFEFEFEFEFEFEULL, /* Down-left. */
		0xFEFEFEFEFEFEFEFEULL, /* Left. */
		0xFEFEFEFEFEFEFE00ULL /* Up-left. */
	};
	static const uint64_t LSHIFTS[] = {
		8, /* Up. */
		7, /* Up-right. */
		0, /* Right. */
		0, /* Down-right. */
		0, /* Down. */
		0, /* Down-left. */
		1, /* Left. */
		9  /* Up-left. */
	};
	static const uint64_t RSHIFTS[] = {
		0, /* Up. */
		0, /* Up-right. */
		1, /* Right. */
		9, /* Down-right. */
		8, /* Down. */
		7, /* Down-left. */
		0, /* Left. */
		0  /* Up-left. */
	};

	// Let's not waste calculations, shifts with 0 are id anyway
	board >>= RSHIFTS[direction];
	board <<= LSHIFTS[direction];
	board &= MASKS[direction];
	return board;
}

/**
 * Generate all valid moves
 *
 * @param The board
 *
 * Based on https://www.hanshq.net/othello.html however, with some things
 * changed.
 */
static uint64_t get_valid_moves(board_t board)
{
	uint64_t t_board;
	uint64_t empty_cells = ~(board.player | board.opponent);
	uint64_t legal_moves = 0;

	for (direction_t d = Up; d != UpLeft; d++) {
		/* Get opponent disks adjacent to my disks in direction dir. */
		t_board = shift(board.player, d) & board.opponent;

		/* Add opponent disks adjacent to those, and so on. */
		t_board |= shift(t_board, d) & board.opponent;
		t_board |= shift(t_board, d) & board.opponent;
		t_board |= shift(t_board, d) & board.opponent;
		t_board |= shift(t_board, d) & board.opponent;
		t_board |= shift(t_board, d) & board.opponent;

		/* Empty cells adjacent to those are valid moves. */
		legal_moves |= shift(t_board, d) & empty_cells;
	}

	return legal_moves;
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
 * Print a graphical representation of a board
 */
void print_board(uint64_t board) {
	// Duplicate horizontal bars because our pieces are double-width
	for (int8_t y = 63; y >= 0; y -= 8) {
		printf("  -----------------\n");
		printf("%" PRIu8 " ", (7 - (y / 8)) + 1);
		for (int8_t x = y; x >= y - 7; x--) {
			printf("|");
			if (is_piece(board, x)) {
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
 * Place a piece on the field. First performs check to see if the field is not
 * already occupied.
 *
 * @param[in,out] The board on which the move should be made
 * @param[in] The coordinate of the desired location
 * @param[in] What should happen when the move is made
 */
void do_move(board_t *board, uint8_t coordinate) {
	debug_print("Placing piece at: %" PRIu8 "\n", coordinate);

	if (is_piece(board->player | board->opponent, coordinate)) {
		printf("ERROR: Tried to place piece on occupied field\n");
		exit(EXIT_FAILURE);
	}

	uint64_t x, bounding_disk;
	uint64_t new_disk = 1ULL << coordinate;
	uint64_t captured_disks = 0;

	place_piece(&board->player, coordinate);

	for (direction_t d = Up; d != UpLeft; d++) {
		/* Find opponent disk adjacent to the new disk. */
		x = shift(new_disk, d) & board->opponent;

		/* Add any adjacent opponent disk to that one, and so on. */
		x |= shift(x, d) & board->opponent;
		x |= shift(x, d) & board->opponent;
		x |= shift(x, d) & board->opponent;
		x |= shift(x, d) & board->opponent;
		x |= shift(x, d) & board->opponent;

		/* Determine whether the disks were captured. */
		bounding_disk = shift(x, d) & board->player;
		captured_disks |= (bounding_disk ? x : 0);
	}

	board->player ^= captured_disks;
	board->opponent ^= captured_disks;
}

/**
 * Checks if the current player can perform ANY move.
 */
bool any_move_valid(uint64_t valid_moves) {
	return valid_moves > 0;
}

#endif
