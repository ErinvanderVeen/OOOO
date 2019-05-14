CC = g++
CFLAGS = -Wall -Wextra -march=native -fPIC -lm -std=c++17 -lstdc++

serial: CFLAGS += -Ofast
serial: othello

serialdebug: CFLAGS += -g -DDEBUG
serialdebug: othello

parallel: CFLAGS += -fopenmp -Ofast -lpthread -DPARALLEL
parallel: othello

paralleldebug: CFLAGS += -fopenmp -g -lpthread -DPARALLEL -DDEBUG
paralleldebug: othello

# Passing this flag will enable performance metrics
metrics: CFLAGS += -fopenmp -g -Ofast -DPARALLEL -DMETRICS
metrics: othello

othello: othello.cpp ai.o state_t.o
	$(CC) $(CFLAGS) othello.cpp ai.o state_t.o eval_hashmap.o -o othello.out

ai.o: ai.cpp ai.hpp state_t.o eval_hashmap.o
	$(CC) $(CFLAGS) -c ai.cpp

state_t.o: state_t.cpp state_t.hpp
	$(CC) $(CFLAGS) -c state_t.cpp

eval_hashmap.o: eval_hashmap.cpp eval_hashmap.hpp
	$(CC) $(CFLAGS) -c eval_hashmap.cpp

run: all
	./othello.out

clean:
	rm *.o; rm *.out
