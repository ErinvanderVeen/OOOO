#ifndef AI_H
#define AI_H

#include <stdlib.h>
#include <inttypes.h>

#include "state_t.h"
#include "debug.h"

coordinate_t ai_turn(void) {
	//uint8_t choice = 0;
	//coordinate_t move = possible_moves[choice];
	//debug_print("Coordinate entered. Column: %c Row: %" PRIu8 "\n", move.column + 97, move.row + 1);
	//return move;
	coordinate_t move;
	move.column = 0;
	move.row = 0;
	return move;
}

#endif
