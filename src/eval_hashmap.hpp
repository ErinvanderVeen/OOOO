#ifndef EVAL_HASHMAP_H
#define EVAL_HASHMAP_H

#include <uthash.h>

#include "state_t.hpp"

typedef struct {
	board_t board;
	double value;
	uint8_t depth;
} board_eval_t;

void add_eval(board_eval_t *eval);
board_eval_t *find_eval(board_t board);
void delete_eval(board_eval_t *eval);
void init_map(void);
void clear_map(void);
void free_map(void);
void print_map(void);

#endif
