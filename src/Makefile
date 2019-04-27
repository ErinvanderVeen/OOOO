CC = clang
CFLAGS = -Wall -Wextra -march=native

all: CFLAGS += -Ofast -DPERFORMANCE
all: othello

debug: CFLAGS += -g -DDEBUG
debug: othello

othello: othello.c ai.o state_t.o
	$(CC) $(CFLAGS) othello.c ai.o state_t.o -o othello.out

ai.o: ai.c ai.h state_t.o
	$(CC) $(CFLAGS) -c ai.c

state_t.o: state_t.c state_t.h
	$(CC) $(CFLAGS) -c state_t.c

run: all
	./othello.out

clean:
	rm *.out
	rm *.o
