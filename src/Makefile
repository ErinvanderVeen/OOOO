CC = clang
CFLAGS = -Wall -Wextra -march=native

all: reversi.c state_t.h ai.h
	$(CC) $(CFLAGS) -Ofast reversi.c -o reversi.out

debug: reversi.c state_t.h ai.h
	$(CC) $(CFLAGS) -fsanitize=address -g reversi.c -o reversi.out -DDEBUG

test: debug
	./reversi.out < tests/t1

run: all
	./reversi.out

clean:
	rm *.out
