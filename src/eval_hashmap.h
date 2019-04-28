#ifndef EVAL_HASHMAP_H
#define EVAL_HASHMAP_H

#include <uthash.h>

#include "state_t.h"

typedef struct {
	board_t board;
	double value;
	uint8_t depth;
	UT_hash_handle hh;
} board_eval_t;

void add_eval(board_eval_t *eval);
board_eval_t *find_eval(board_t board);
void delete_eval(board_eval_t *eval);
void init_map(void);
void free_map(void);

#endif
