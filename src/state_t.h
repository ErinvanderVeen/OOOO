#ifndef STATE_T_H
#define STATE_T_H

#include "debug.h"

typedef enum {E = 0, SE, S, SW, W, NW, N, NE} dir_t;

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
 * Checks if the specified bit is set.
 *
 * @param[in] Number to check on
 * @param[in] The bit number to check
 */
bool is_set(uint64_t number, uint8_t n) {
    return (number & 1UL << n) != 0;
}

/**
 * Places a piece on the specified location of the specified board
 *
 * @param[in,out] Board that should be checked for a piece
 * @param[in] The coordinate of the desired location
 */
void set(uint64_t *number, uint8_t n) {
    *number |= 1UL << n;
}

/**
 * Removes the piece of the board on the specified location
 *
 * @param[in,out] The board of which the piece must be removed
 * @param[in] The coordinate of the desired location
 */
void clear(uint64_t* number, uint8_t n) {
    *number &= ~(1UL << n);
}

/**
 * Count the number of set bits in a 64-bit number
 */
uint8_t count(uint64_t number) {
    return __builtin_popcountll(number);
}

/**
 * Inspiration from https://www.hanshq.net/othello.html.
 */
uint64_t shift(uint64_t board, dir_t dir) {
    static const uint64_t MASKS[] = {
            0x7F7F7F7F7F7F7F7FULL, /* Right. */
            0x007F7F7F7F7F7F7FULL, /* Down-right. */
            0xFFFFFFFFFFFFFFFFULL, /* Down. */
            0x00FEFEFEFEFEFEFEULL, /* Down-left. */
            0xFEFEFEFEFEFEFEFEULL, /* Left. */
            0xFEFEFEFEFEFEFE00ULL, /* Up-left. */
            0xFFFFFFFFFFFFFFFFULL, /* Up. */
            0x7F7F7F7F7F7F7F00ULL  /* Up-right. */
    };
    static const uint64_t LSHIFTS[] = {
            0, /* Right. */
            0, /* Down-right. */
            0, /* Down. */
            0, /* Down-left. */
            1, /* Left. */
            9, /* Up-left. */
            8, /* Up. */
            7  /* Up-right. */
    };
    static const uint64_t RSHIFTS[] = {
            1, /* Right. */
            9, /* Down-right. */
            8, /* Down. */
            7, /* Down-left. */
            0, /* Left. */
            0, /* Up-left. */
            0, /* Up. */
            0  /* Up-right. */
    };
    return (dir < 8 / 2) ? ((board >> RSHIFTS[dir]) & MASKS[dir]) : ((board << LSHIFTS[dir]) & MASKS[dir]);
}

/**
 *
 * @param[in,out] The board on which the move should be made
 * @param[in] The coordinate of the desired location
 */
void do_move(board_t *board, uint8_t coordinate) {
    debug_print("Placing piece at: %" PRIu8 "\n", coordinate);

    if (is_set(board->player | board->opponent, coordinate)) {
        printf("ERROR: Tried to place piece on occupied field\n");
        exit(EXIT_FAILURE);
    }

    uint64_t captured = 0;
    uint64_t move = 0;
    uint64_t bounding, x;

    set(&move, coordinate);
    set(&board->player, coordinate);

    for (int i = 0; i < 8; ++i) {
        x = shift(move, i) & board->opponent;

        x |= shift(x, i) & board->opponent;
        x |= shift(x, i) & board->opponent;
        x |= shift(x, i) & board->opponent;
        x |= shift(x, i) & board->opponent;
        x |= shift(x, i) & board->opponent;

        bounding = shift(x, i) & board->player;
        captured |= (bounding ? x : 0);
    }

    board->player ^= captured;
    board->opponent ^= captured;
}

uint64_t get_valid_moves(board_t board) {
    uint64_t empty = ~(board.player | board.opponent);
    uint64_t legal = 0;
    uint64_t x;

    for (int i = 0; i < 8; ++i) {
        x = shift(board.player, i) & board.opponent;

        x |= shift(x, i) & board.opponent;
        x |= shift(x, i) & board.opponent;
        x |= shift(x, i) & board.opponent;
        x |= shift(x, i) & board.opponent;
        x |= shift(x, i) & board.opponent;

        legal |= shift(x, i) & empty;
    }

    return legal;
}

/**
 * Check if the move is a valid move. NOTE: Does not perform a lookup in the
 * table, but calculates the value itself. Should be used to update the table.
 *
 * @param[in] The board on which we should check the validity
 * @param[in] The coordinate of the desired location
 * @param[out] Contains the pieces that should be flipped when this move turns out to be valid
 */
bool has_valid_move(board_t board) {
    return get_valid_moves(board) > 0;
}

/**
 * Check if the move is a valid move. NOTE: Does not perform a lookup in the
 * table, but calculates the value itself. Should be used to update the table.
 *
 * @param[in] The board on which we should check the validity
 * @param[in] The coordinate of the desired location
 * @param[out] Contains the pieces that should be flipped when this move turns out to be valid
 */
bool is_valid_move(board_t board, uint8_t coordinate) {
    return is_set(get_valid_moves(board), coordinate);
}

void switch_boards(board_t *board) {
    uint64_t temp = board->player;
    board->player = board->opponent;
    board->opponent = temp;
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
            if (is_set(board.player, x)) {
                printf("P");
            } else if (is_set(board.opponent, x)) {
                printf("o");
            } else if (is_set(valid_moves, x) && show_valid_moves) {
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
            if (is_set(board, x)) {
                printf("*");
            } else {
                printf(" ");
            }
        }
        printf("|\n");
    }
    printf("   a b c d e f g h\n");
}

#endif
