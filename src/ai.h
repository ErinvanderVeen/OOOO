#ifndef AI_H
#define AI_H

#include <inttypes.h>
#include <stdlib.h>

#include "state_t.h"

/**
 * Performs negamax on the provided board. Negamax is an algorithm that 
 *
 * @param board
 * @param depth
 * @param alpha
 * @param beta
 * @param player -  the current player to consider. 1 is the player, -1 is the opponent
 * @return
 */
double negamax(board_t board, uint64_t depth, double alpha, double beta, int8_t player);

uint8_t ai_turn(board_t board);

#endif
