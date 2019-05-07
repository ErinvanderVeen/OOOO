#include "eval_hashmap.hpp"
#include "debug.hpp"

#include <pthread.h>

#define HASHSIZE 24
#define SIZEMASK 0xffffff
#define LOCK_COUNT 128

static board_eval_t **hashtable;
static uint64_t vals[64][2];

#ifdef PARALLEL
static pthread_rwlock_t *locks;
#endif

#ifdef METRICS
static uint64_t collisions;
static uint64_t total_hits;
static uint64_t total_misses;
#endif

uint64_t hash(board_t board) {
	uint64_t hash = 0;
	hash = (board.opponent ^ vals[count(board.opponent)][1]);
	hash |= (board.player ^ vals[count(board.player)][1]);

	return (hash & SIZEMASK);
}

void add_eval(board_eval_t *eval) {
	uint64_t hashcode = hash(eval->board);

#ifdef PARALLEL
	pthread_rwlock_wrlock(&locks[hashcode % LOCK_COUNT]);
#endif
#ifdef METRICS
	board_eval_t *exists = hashtable[hash(eval->board)];
	if (exists != NULL)
		collisions++;
#endif
	hashtable[hashcode] = eval;
#ifdef PARALLEL
	pthread_rwlock_unlock(&locks[hashcode % LOCK_COUNT]);
#endif
}

board_eval_t *find_eval(board_t board) {
	board_eval_t *eval;
	uint64_t hashcode = hash(board);

#ifdef PARALLEL
	pthread_rwlock_rdlock(&locks[hashcode % LOCK_COUNT]);
#endif
	eval = hashtable[hashcode];
#ifdef PARALLEL
	pthread_rwlock_unlock(&locks[hashcode % LOCK_COUNT]);
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
	uint64_t hashcode = hash(eval->board);
#ifdef PARALLEL
	pthread_rwlock_wrlock(&locks[hashcode % LOCK_COUNT]);
#endif
	hashtable[hashcode] = NULL;
#ifdef PARALLEL
	pthread_rwlock_unlock(&locks[hashcode % LOCK_COUNT]);
#endif
}

void init_map(void) {
	if (hashtable == NULL)
		hashtable = (board_eval_t **) calloc(1 << HASHSIZE, sizeof(board_eval_t *));

#ifdef PARALLEL
	if (locks == NULL) {
		locks = (pthread_rwlock_t *) calloc(LOCK_COUNT, sizeof(pthread_rwlock_t));
		for (int i = 0; i < LOCK_COUNT; ++i) {
			pthread_rwlock_init(&locks[i], NULL);
		}
	}
#endif

	for (int i = 0; i < 64; ++i) {
		for (int j = 0; j < 2; ++j) {
			vals[i][j] = rand();
		}
	}

#ifdef METRICS
	total_hits = 0;
	total_misses = 0;
	collisions = 0;
#endif
}

void clear_map(void) {
	for (int i = 0; i < 1 << HASHSIZE; ++i) {
		if (hashtable[i] != NULL)
			free(hashtable[i]);
	}
	memset(hashtable, '\0', (1 << HASHSIZE) * sizeof(board_eval_t *));

#ifdef METRICS
	printf("HASHMAP:\n");
	printf("\t Total Hits: %" PRIu64 "\n", total_hits);
	printf("\t Total Misses: %" PRIu64 "\n", total_misses);
	printf("\t Total Collisions: %" PRIu64 "\n", collisions);
	total_hits = 0;
	total_misses = 0;
	collisions = 0;
#endif
}

void print_map(void) {
#ifdef METRICS
	printf("HASHMAP:\n");
	printf("\t Total Hits: %" PRIu64 "\n", total_hits);
	printf("\t Total Misses: %" PRIu64 "\n", total_misses);
	printf("\t Total Collisions: %" PRIu64 "\n", collisions);
#endif
}

void free_map(void) {
#ifdef PARALLEL
	for (int i = 0; i < LOCK_COUNT; ++i) {
		pthread_rwlock_destroy(&locks[i]);
	}
	free(locks);
#endif

	for (int i = 0; i < 1 << HASHSIZE; ++i) {
		if (hashtable[i] != NULL)
			free(hashtable[i]);
	}

	free(hashtable);
}
