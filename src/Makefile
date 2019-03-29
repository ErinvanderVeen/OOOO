all: reversi.c state.h
	$(CC) -Wall -Wextra -Ofast reversi.c -o reversi.out

debug: reversi.c state.h
	$(CC) -Wall -Wextra -g reversi.c -o reversi.out

test: debug
	./reversi.out < tests/t1

run: all
	./reversi.out

clean:
	rm *.out
