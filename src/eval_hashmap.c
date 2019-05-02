#include "eval_hashmap.h"

#include <omp.h>

#include "debug.h"

static board_eval_t *eval_hashmap = NULL;
#ifdef PARALLEL
static omp_lock_t maplock;
#endif

void add_eval(board_eval_t *eval) {
#ifdef PARALLEL
	omp_set_lock(&maplock);
#endif
	HASH_ADD(hh, eval_hashmap, board, sizeof(board_t), eval);
#ifdef PARALLEL
	omp_unset_lock(&maplock);
#endif
}

board_eval_t *find_eval(board_t board) {
	board_eval_t *eval;

#ifdef PARALLEL
	omp_set_lock(&maplock);
#endif
	HASH_FIND(hh, eval_hashmap, &board, sizeof(board_t), eval);
#ifdef PARALLEL
	omp_unset_lock(&maplock);
#endif

	return eval;
}

void delete_eval(board_eval_t *eval) {
#ifdef PARALLEL
	omp_set_lock(&maplock);
#endif
	HASH_DEL(eval_hashmap, eval);
#ifdef PARALLEL
	omp_unset_lock(&maplock);
#endif
}

void init_map(void) {
#ifdef PARALLEL
	omp_init_lock(&maplock);
#endif
}

void free_map(void) {
	board_eval_t *cur, *tmp;

	HASH_ITER(hh, eval_hashmap, cur, tmp) {
		HASH_DEL(eval_hashmap, cur);
		free(cur);
	}

#ifdef PARALLEL
	omp_destroy_lock(&maplock);
#endif
}
