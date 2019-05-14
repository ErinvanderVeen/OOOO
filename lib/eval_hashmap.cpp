#include "eval_hashmap.hpp"

#include <omp.h>

#include "debug.hpp"

static board_eval_t *eval_hashmap = NULL;
#ifdef PARALLEL
static omp_lock_t maplock;
#endif

#ifdef METRICS
static uint64_t total_hits = 0;
static uint64_t total_misses = 0;
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

#ifdef METRICS
	if (eval == NULL)
		total_misses++;
	else
		total_hits++;
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

void print_hash_metrics(void) {
#ifdef METRICS
	printf("HASHMAP:\n");
	printf("    Total Hits: %" PRIu64 "\n", total_hits);
	printf("    Total Misses: %" PRIu64 "\n", total_misses);
	printf("    %% Hit: %f\n", 100.0 * ((double) total_hits) / ((double) total_hits + (double) total_misses));
#endif
}