#ifndef STATE_H
#define STATE_H

typedef enum {Human, Computer} player_t;

// A more optimal solution would be to use 2 bits per cell, 4 cells per byte.
typedef enum {Black, White, None} color_t;

player_t white = Human;
player_t black = Human;

typedef struct {
	color_t turn;
	color_t board[8][8];
	bool valid_moves[8][8];
} state_t;

state_t state;


state_t default_state(void) {
	state_t temp = {
		.turn = Black,
		.board = {
			{ None, None, None, None,  None,  None, None, None },
			{ None, None, None, None,  None,  None, None, None },
			{ None, None, None, None,  None,  None, None, None },
			{ None, None, None, White, Black, None, None, None },
			{ None, None, None, Black, White, None, None, None },
			{ None, None, None, None,  None,  None, None, None },
			{ None, None, None, None,  None,  None, None, None },
			{ None, None, None, None,  None,  None, None, None },
		},
		.valid_moves = {
			{ false, false, false, false, false, false, false, false },
			{ false, false, false, false, false, false, false, false },
			{ false, false, false, true,  false, false, false, false },
			{ false, false, true,  false, false, false, false, false },
			{ false, false, false, false, false, true,  false, false },
			{ false, false, false, false, true,  false, false, false },
			{ false, false, false, false, false, false, false, false },
			{ false, false, false, false, false, false, false, false },
		}
	};
	return temp;
}

static inline color_t opposite_color(color_t c) {
	if (c == White)
		return Black;
	return White;
}

void print_line(uint8_t y) {
	printf("%" PRIu8 " ", y + 1);
	for (uint8_t x = 0; x < 8; x++) {
		printf("\u2502");
		switch (state.board[x][y]) {
			case Black:
				printf("\u26AA");
				break;
			case White:
				printf("\u26AB");
				break;
			default:
				if (state.valid_moves[x][y])
					printf("\u25A1 ");
				else
					printf("  ");
				break;
		}
	}
	printf("\u2502\n");
}

void print_state(void) {
	// Duplicate horizontal bars because our pieces are double-width
	printf("\n  \u250C\u2500\u2500\u252C\u2500\u2500\u252C\u2500\u2500\u252C\u2500\u2500\u252C\u2500\u2500\u252C\u2500\u2500\u252C\u2500\u2500\u252C\u2500\u2500\u2510\n");
	print_line(0);
	for (uint8_t y = 1; y < 8; y++) {
		printf("  \u251C\u2500\u2500\u253C\u2500\u2500\u253C\u2500\u2500\u253C\u2500\u2500\u253C\u2500\u2500\u253C\u2500\u2500\u253C\u2500\u2500\u253C\u2500\u2500\u2524\n");
		print_line(y);
	}
	printf("  \u2514\u2500\u2500\u2534\u2500\u2500\u2534\u2500\u2500\u2534\u2500\u2500\u2534\u2500\u2500\u2534\u2500\u2500\u2534\u2500\u2500\u2534\u2500\u2500\u2518\n");
	printf("   a  b  c  d  e  f  g  h\n");
}

void flip_left(uint8_t column, uint8_t row, color_t color, color_t op_color) {
	do {
		column--;
	} while (state.board[column][row] == op_color && column > 0);

	if (state.board[column][row] == color) {
		column++;
		for(; state.board[column][row] == op_color; column++)
			state.board[column][row] = color;
	}
}

void flip_right(uint8_t column, uint8_t row, color_t color, color_t op_color) {
	do {
		column++;
	} while (state.board[column][row] == op_color && column < 7);

	if (state.board[column][row] == color) {
		column--;
		for(; state.board[column][row] == op_color; column--)
			state.board[column][row] = color;
	}
}

void flip_up(uint8_t column, uint8_t row, color_t color, color_t op_color) {
	do {
		row--;
	} while (state.board[column][row] == op_color && row > 0);

	if (state.board[column][row] == color) {
		row++;
		for(; state.board[column][row] == op_color; row++)
			state.board[column][row] = color;
	}
}

void flip_down(uint8_t column, uint8_t row, color_t color, color_t op_color) {
	do {
		row++;
	} while (state.board[column][row] == op_color && row < 7);

	if (state.board[column][row] == color) {
		row--;
		for(; state.board[column][row] == op_color; row--)
			state.board[column][row] = color;
	}
}

void flip_left_up(uint8_t column, uint8_t row, color_t color, color_t op_color) {
	do {
		column--;
		row--;
	} while (state.board[column][row] == op_color && row > 0 && column > 0);

	if (state.board[column][row] == color) {
		column++;
		row++;
		for(; state.board[column][row] == op_color; row++, column++)
			state.board[column][row] = color;
	}
}

void flip_right_up(uint8_t column, uint8_t row, color_t color, color_t op_color) {
	do {
		column++;
		row--;
	} while (state.board[column][row] == op_color && row > 0 && column < 7);

	if (state.board[column][row] == color) {
		column--;
		row++;
		for(; state.board[column][row] == op_color; row++, column--)
			state.board[column][row] = color;
	}
}

void flip_left_down(uint8_t column, uint8_t row, color_t color, color_t op_color) {
	do {
		column--;
		row++;
	} while (state.board[column][row] == op_color && row < 7 && column > 0);

	if (state.board[column][row] == color) {
		column++;
		row--;
		for(; state.board[column][row] == op_color; row--, column++)
			state.board[column][row] = color;
	}
}
void flip_right_down(uint8_t column, uint8_t row, color_t color, color_t op_color) {
	do {
		column++;
		row++;
	} while (state.board[column][row] == op_color && row < 7 && column < 7);

	if (state.board[column][row] == color) {
		column--;
		row--;
		for(; state.board[column][row] == op_color; row--, column--)
			state.board[column][row] = color;
	}
}

void flip_neighbours(uint8_t column, uint8_t row, color_t color) {
	color_t op_color = opposite_color(color);

	if (column != 0)
		flip_left(column, row, color, op_color);
	if (column != 7)
		flip_right(column, row, color, op_color);
	if (row != 0)
		flip_up(column, row, color, op_color);
	if (row != 7)
		flip_down(column, row, color, op_color);
	if (column != 0 && row != 0)
		flip_left_up(column, row, color, op_color);
	if (column != 7 && row != 0)
		flip_right_up(column, row, color, op_color);
	if (column != 0 && row != 7)
		flip_left_down(column, row, color, op_color);
	if (column != 7 && row != 7)
		flip_right_down(column, row, color, op_color);
}

void place_piece(uint8_t column, uint8_t row, color_t color) {
	if (state.board[column][row] != None) {
		printf("ERROR: Tried to place piece on ccupied field\n");
		exit(EXIT_FAILURE);
	}

	state.board[column][row] = color;

	flip_neighbours(column, row, color);
}

bool valid_left(uint8_t column, uint8_t row, color_t op_color) {
	if (column <= 1)
		return false;

	column--;

	if (state.board[column][row] == op_color) {
		do {
			column--;
		} while (column > 0 && state.board[column][row] == op_color);

		if (state.board[column][row] == state.turn)
			return true;
	}

	return false;
}

bool valid_right(uint8_t column, uint8_t row, color_t op_color) {
	if (column >= 6)
		return false;

	column++;

	if (state.board[column][row] == op_color) {
		do {
			column++;
		} while (column < 7 && state.board[column][row] == op_color);

		if (state.board[column][row] == state.turn)
			return true;
	}

	return false;
}

bool valid_up(uint8_t column, uint8_t row, color_t op_color) {
	if (row <= 1)
		return false;

	row--;

	if (state.board[column][row] == op_color) {
		do {
			row--;
		} while (row > 0 && state.board[column][row] == op_color);

		if (state.board[column][row] == state.turn)
			return true;
	}

	return false;
}

bool valid_down(uint8_t column, uint8_t row, color_t op_color) {
	if (row >= 6)
		return false;

	row++;

	if (state.board[column][row] == op_color) {
		do {
			row++;
		} while (row < 7 && state.board[column][row] == op_color);

		if (state.board[column][row] == state.turn)
			return true;
	}

	return false;
}

bool valid_left_up(uint8_t column, uint8_t row, color_t op_color) {
	if (column <= 1 || row <= 1)
		return false;

	column--;
	row--;

	if (state.board[column][row] == op_color) {
		do {
			column--;
			row--;
		} while (column > 0 && row > 0 && state.board[column][row] == op_color);

		if (state.board[column][row] == state.turn)
			return true;
	}

	return false;
}

bool valid_right_up(uint8_t column, uint8_t row, color_t op_color) {
	if (column >= 6 || row <= 1)
		return false;

	column++;
	row--;

	if (state.board[column][row] == op_color) {
		do {
			column++;
			row--;
		} while (column < 7 && row > 0&& state.board[column][row] == op_color);

		if (state.board[column][row] == state.turn)
			return true;
	}

	return false;
}

bool valid_left_down(uint8_t column, uint8_t row, color_t op_color) {
	if (column <= 1 || row >= 6)
		return false;

	column--;
	row++;

	if (state.board[column][row] == op_color) {
		do {
			column--;
			row++;
		} while (column > 0 && row < 7 && state.board[column][row] == op_color);

		if (state.board[column][row] == state.turn)
			return true;
	}

	return false;
}

bool valid_right_down(uint8_t column, uint8_t row, color_t op_color) {
	if (column >= 6 || row >= 6)
		return false;

	column++;
	row++;

	if (state.board[column][row] == op_color) {
		do {
			column++;
			row++;
		} while (column < 7 && row < 7 && state.board[column][row] == op_color);

		if (state.board[column][row] == state.turn)
			return true;
	}

	return false;
}

bool is_valid_move(uint8_t column, uint8_t row) {
	if (state.board[column][row] != None)
		return false;

	color_t op_color = opposite_color(state.turn);
	return (
		valid_left(column, row, op_color) ||
		valid_right(column, row, op_color) ||
		valid_up(column, row, op_color) ||
		valid_down(column, row, op_color) ||
		valid_left_up(column, row, op_color) ||
		valid_right_up(column, row, op_color) ||
		valid_left_down(column, row, op_color) ||
		valid_right_down(column, row, op_color)
	);
}

void update_valid_moves(void) {
	for (uint8_t column = 0; column < 8; column++) {
		for (uint8_t row = 0; row < 8; row++) {
			state.valid_moves[column][row] = is_valid_move(column, row);
		}
	}
}

// Checks a move is possible in any field
bool any_move_valid(void) {
	// Ideally I would calculate this once, and store the possible moves
	// This would then, later, allow for reuse in validating user input
	// TODO
	return true;
}
#endif
