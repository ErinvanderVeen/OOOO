#include "eval_hashmap.hpp"
#include "debug.hpp"

#include <pthread.h>

// Define the size of the hashtable, 2^hashsize
#define HASHSIZE 24

// Bitmask used to mask out as many bits as the size from big numbers
#define SIZEMASK 0xffffff

// Define how many locks to use. Should be a multiple of 2
#define LOCK_COUNT 128

static board_eval_t **hashtable;
static uint64_t vals[64];

#ifdef PARALLEL
static pthread_rwlock_t *locks;
#endif

#ifdef METRICS
static uint64_t collisions;
static uint64_t total_hits = 0;
static uint64_t total_misses = 0;
static uint64_t total_hits;
static uint64_t total_misses;
#endif

/*
 * Hash function that seems to result in very few collisions.
 */
uint64_t hash(board_t board) {
	uint64_t hash = 0;
	hash = (board.opponent ^ vals[count(board.opponent)]);
	hash |= (board.player ^ vals[count(board.player)]);

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
		vals[i]= rand();
	}

}

void clear_map(void) {
	for (int i = 0; i < 1 << HASHSIZE; ++i) {
		if (hashtable[i] != NULL)
			free(hashtable[i]);
	}
	memset(hashtable, '\0', (1 << HASHSIZE) * sizeof(board_eval_t *));

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
	printf("    Total Collisions: %" PRIu64 "\n", collisions);
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
