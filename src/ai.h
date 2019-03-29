#ifndef AI_H
#define AI_H

#include <stdlib.h>
#include <inttypes.h>

#include "state.h"
#include "debug.h"

coordinate_t ai_turn(void) {
	uint8_t choice = (uint8_t) rand() % nr_possible_moves;
	coordinate_t move = possible_moves[choice];
	debug_print("Coordinate entered. Column: %c Row: %" PRIu8 "\n", move.column + 97, move.row + 1);
	return move;
}

#endif
