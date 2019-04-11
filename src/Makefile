CC = clang
CFLAGS = -Wall -Wextra

all: reversi.c state_t.h ai.h
	$(CC) $(CFLAGS) -Ofast reversi.c -o reversi.out
	$(CC) $(CFLAGS) -Ofast evaluator.c -o evaluator.out

debug: reversi.c state_t.h ai.h
	$(CC) $(CFLAGS) -fsanitize=address -g reversi.c -o reversi.out -DDEBUG

test: debug
	./reversi.out < tests/t1

run: all
	./reversi.out

eval: all
	./evaluator.out

clean:
	rm *.out
