#include "eval_hashmap.h"

#include "debug.h"

board_eval_t *eval_hashmap = NULL;

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
