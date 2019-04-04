#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG 1
#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

bool is_piece_debug(uint8_t column, uint8_t row, uint64_t board) {
	uint8_t column_mask = (uint64_t) 1 << (7 - column);
	uint8_t column_val = board >> ((7 - row) * 8);
	return (column_mask & column_val) > 0;
}

/**
 * Print a graphical representation of a row in the field.
 *
 * @param[in] The row
 * @param[in] Wether or not valid moves should be highlighted
 */
void print_line_debug(uint8_t y, uint64_t board) {
	printf("%" PRIu8 " ", y + 1);
	for (uint8_t x = 0; x < 8; x++) {
		printf("\u2502");
		if (is_piece_debug(x, y, board)) {
			printf("\u26AA");
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
void print_state_debug(uint64_t board) {
	// Duplicate horizontal bars because our pieces are double-width
	printf("\n  \u250C\u2500\u2500\u252C\u2500\u2500\u252C\u2500\u2500\u252C\u2500\u2500\u252C\u2500\u2500\u252C\u2500\u2500\u252C\u2500\u2500\u252C\u2500\u2500\u2510\n");
	print_line_debug(0, board);
	for (uint8_t y = 1; y < 8; y++) {
		printf("  \u251C\u2500\u2500\u253C\u2500\u2500\u253C\u2500\u2500\u253C\u2500\u2500\u253C\u2500\u2500\u253C\u2500\u2500\u253C\u2500\u2500\u253C\u2500\u2500\u2524\n");
		print_line_debug(y, board);
	}
	printf("  \u2514\u2500\u2500\u2534\u2500\u2500\u2534\u2500\u2500\u2534\u2500\u2500\u2534\u2500\u2500\u2534\u2500\u2500\u2534\u2500\u2500\u2534\u2500\u2500\u2518\n");
	printf("   a  b  c  d  e  f  g  h\n");
}

#endif
