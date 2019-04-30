#include "eval_hashmap.h"

#include <omp.h>

#include "debug.h"

static board_eval_t *eval_hashmap = NULL;
static omp_lock_t maplock;

void add_eval(board_eval_t *eval) {
	omp_set_lock(&maplock);
	HASH_ADD(hh, eval_hashmap, board, sizeof(board_t), eval);
	omp_unset_lock(&maplock);
}

board_eval_t *find_eval(board_t board) {
	board_eval_t *eval;

	omp_set_lock(&maplock);
	HASH_FIND(hh, eval_hashmap, &board, sizeof(board_t), eval);
	omp_unset_lock(&maplock);

	return eval;
}

void delete_eval(board_eval_t *eval) {
	omp_set_lock(&maplock);
	HASH_DEL(eval_hashmap, eval);
	omp_unset_lock(&maplock);
}

void init_map(void) {
	omp_init_lock(&maplock);
}

void free_map(void) {
	board_eval_t *cur, *tmp;

	HASH_ITER(hh, eval_hashmap, cur, tmp) {
		HASH_DEL(eval_hashmap, cur);
		free(cur);
	}

	omp_destroy_lock(&maplock);
}
