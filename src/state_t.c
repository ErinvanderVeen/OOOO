#include "state_t.h"

#include "debug.h"

#define ONE (uint64_t) 1

bool is_set(uint64_t number, uint8_t n) {
	return (number & ONE << n) != 0;
}

void set(uint64_t *number, uint8_t n) {
	*number |= ONE << n;
}

void clear(uint64_t *number, uint8_t n) {
	*number &= ~(ONE << n);
}

uint8_t count(uint64_t number) {
	return __builtin_popcountll(number);
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

void do_move(board_t *board, uint8_t coordinate) {
	debug_print("Placing piece at: %" PRIu8 "\n", coordinate);

	if (is_set(board->player | board->opponent, coordinate)) {
		printf("ERROR: Tried to place piece on occupied field\n");
		exit(EXIT_FAILURE);
	}

	uint64_t x, bounding_disk;
	uint64_t new_disk = 0;
	uint64_t captured_disks = 0;

	set(&new_disk, coordinate);
	set(&board->player, coordinate);

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

uint64_t get_valid_moves(board_t board) {
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

bool has_valid_move(board_t board) {
	return get_valid_moves(board) > 0;
}

bool is_valid_move(board_t board, uint8_t coordinate) {
	return is_set(get_valid_moves(board), coordinate);
}

void switch_boards(board_t *board) {
	uint64_t temp = board->player;
	board->player = board->opponent;
	board->opponent = temp;
}

void print_state(board_t board, uint64_t valid_moves, bool show_valid_moves) {
	// Duplicate horizontal bars because our pieces are double-width
	for (int8_t y = 63; y >= 0; y -= 8) {
		printf("  ---------------------------------\n");
		printf("%" PRIu8 " ", (7 - (y / 8)) + 1);
		for (int8_t x = y; x >= y - 7; x--) {
			printf("|");
			if (is_set(board.player, x)) {
				printf(" □ ");
			} else if (is_set(board.opponent, x)) {
				printf(" ■ ");
			} else if (is_set(valid_moves, x) && show_valid_moves) {
				printf(" * ");
			} else {
				printf("   ");
			}
		}
		printf("|\n");
	}
	printf("  ---------------------------------\n");
	printf("    a   b   c   d   e   f   g   h\n");
}

void print_board(uint64_t board) {
	// Duplicate horizontal bars because our pieces are double-width
	for (int8_t y = 63; y >= 0; y -= 8) {
		printf("  ---------------------------------\n");
		printf("%" PRIu8 " ", (7 - (y / 8)) + 1);
		for (int8_t x = y; x >= y - 7; x--) {
			printf("|");
			if (is_set(board, x)) {
				printf(" ■ ");
			} else {
				printf("   ");
			}
		}
		printf("|\n");
	}
	printf("  ---------------------------------\n");
	printf("    a   b   c   d   e   f   g   h\n");
}
