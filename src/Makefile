CC = clang
CFLAGS = -Wall -Wextra

all: reversi.c state.h
	$(CC) $(CFLAGS) -Ofast reversi.c -o reversi.out

debug: reversi.c state.h
	$(CC) $(CFLAGS) -fsanitize=address -g reversi.c -o reversi.out

test: debug
	./reversi.out < tests/t1

run: all
	./reversi.out

clean:
	rm *.out
