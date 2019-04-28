CC = gcc
CFLAGS = -Wall -Wextra -march=native -fopenmp -fPIC -lm

all: CFLAGS += -Ofast -DPERFORMANCE
all: othello

debug: CFLAGS += -g -DDEBUG
debug: othello

othello: othello.c ai.o state_t.o
	$(CC) $(CFLAGS) othello.c ai.o state_t.o eval_hashmap.o -o othello.out

ai.o: ai.c ai.h state_t.o eval_hashmap.o
	$(CC) $(CFLAGS) -c ai.c

state_t.o: state_t.c state_t.h
	$(CC) $(CFLAGS) -c state_t.c

eval_hashmap.o: eval_hashmap.c eval_hashmap.h
	$(CC) $(CFLAGS) -c eval_hashmap.c

run: all
	./othello.out

clean:
	rm *.o
	rm *.out
	rm *.o
