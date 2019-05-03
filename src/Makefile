CC = gcc
CFLAGS = -Wall -Wextra -march=native -fPIC -lm

serial: CFLAGS += -Ofast
serial: othello

serialdebug: CFLAGS += -g -DDEBUG
serialdebug: othello

parallel: CFLAGS += -fopenmp -Ofast -DPARALLEL
parallel: othello

paralleldebug: CFLAGS += -fopenmp -g -DPARALLEL -DDEBUG
paralleldebug: othello

# Passing this flag will enable performance metrics
metrics: CFLAGS += -fopenmp -Ofast -DPARALLEL -DMETRICS
metrics: othello

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
	rm *.o; rm *.out
