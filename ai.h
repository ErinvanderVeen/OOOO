#ifndef AI_H
#define AI_H

#include <stdlib.h>

#include "state.h"

coordinate_t ai_turn(void) {
	uint8_t choice = (uint8_t) rand() % nr_possible_moves;
	return possible_moves[choice];
}

#endif
