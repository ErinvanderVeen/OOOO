#ifndef STATE_T_H
#define STATE_T_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum {
	Up = 0,
	UpRight = 1,
	Right = 2,
	DownRight = 3,
	Down = 4,
	DownLeft = 5,
	Left = 6,
	UpLeft = 7
} direction_t;
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
bool is_set(uint64_t number, uint8_t n);

/**
 * Places a piece on the specified location of the specified board
 *
 * @param[in,out] Board that should be checked for a piece
 * @param[in] The coordinate of the desired location
 */
void set(uint64_t *number, uint8_t n);

/**
 * Removes the piece of the board on the specified location
 *
 * @param[in,out] The board of which the piece must be removed
 * @param[in] The coordinate of the desired location
 */
void clear(uint64_t *number, uint8_t n);

/**
 * Count the number of set bits in a 64-bit number
 */
uint8_t count(uint64_t number);

/**
 * Place a piece on the field. First performs check to see if the field is not
 * already occupied.
 *
 * @param[in,out] The board on which the move should be made
 * @param[in] The coordinate of the desired location
 * @param[in] What should happen when the move is made
 */
void do_move(board_t *board, uint8_t coordinate);

/**
 * Generate all valid moves
 *
 * @param The board
 *
 * Based on https://www.hanshq.net/othello.html however, with some things
 * changed.
 */
uint64_t get_valid_moves(board_t board);

/**
 * Check if the move is a valid move. NOTE: Does not perform a lookup in the
 * table, but calculates the value itself. Should be used to update the table.
 *
 * @param[in] The board on which we should check the validity
 * @param[in] The coordinate of the desired location
 * @param[out] Contains the pieces that should be flipped when this move turns out to be valid
 */
bool has_valid_move(board_t board);

void switch_boards(board_t *board);

/**
 * Print a graphical representation of the entire field
 *
 * @param[in] Wether or not valid moves should be highlighted
 */
void print_state(board_t board, uint64_t valid_moves, bool show_valid_moves);

/**
 * Print a graphical representation of a board
 */
void print_board(uint64_t board);
#endif
