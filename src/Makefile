CC = clang
CFLAGS = -Wall -Wextra -march=native

all: othello.c state_t.h debug.h ai.h
	$(CC) $(CFLAGS) -Ofast othello.c -o othello.out -DPERFORMANCE

debug: othello.c state_t.h debug.h ai.h
	$(CC) $(CFLAGS) -fsanitize=address -g othello.c -o othello.out -DDEBUG -DPERFORMANCE

run: all
	./othello.out

clean:
	rm *.out
