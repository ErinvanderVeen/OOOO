CC = clang
CFLAGS = -Wall -Wextra

all: reversi.c state_t.h ai.h performance.c
	$(CC) $(CFLAGS) -Ofast reversi.c -o reversi.out
	$(CC) $(CFLAGS) -Ofast performance.c -o performance.out -DPERFORMANCE

debug: reversi.c state_t.h ai.h performance.c
	$(CC) $(CFLAGS) -fsanitize=address -g reversi.c -o reversi.out -DDEBUG
	$(CC) $(CFLAGS) -fsanitize=address -g performance.c -o performance.out -DDEBUG -DPERFORMANCE

test: debug
	./reversi.out < tests/t1

run: all
	./reversi.out

performance: all
	./performance.out

clean:
	rm *.out
