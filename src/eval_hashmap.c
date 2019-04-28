#include "eval_hashmap.h"

#include "debug.h"

static board_eval_t *eval_hashmap = NULL;

void add_eval(board_eval_t *eval) {
	HASH_ADD(hh, eval_hashmap, board, sizeof(board_t), eval);
}

board_eval_t *find_eval(board_t board) {
	board_eval_t *eval;

	HASH_FIND(hh, eval_hashmap, &board, sizeof(board_t), eval);

	return eval;
}

void delete_eval(board_eval_t *eval) {
	HASH_DEL(eval_hashmap, eval);
}

void free_map(void) {
	board_eval_t *cur, *tmp;

	HASH_ITER(hh, eval_hashmap, cur, tmp) {
		HASH_DEL(eval_hashmap, cur);
		free(cur);
	}
}
