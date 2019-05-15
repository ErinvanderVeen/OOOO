CC = g++
CFLAGS = -Wall -Wextra -march=native -fPIC -lm -std=c++17 -lstdc++

all: CFLAGS += -Ofast
all: interface

debug: CFLAGS += -g
debug: interface

interface: interface.cpp state_t
	$(CC) $(CFLAGS) interface.cpp ../lib/state_t.o -o interface.out

state_t: ../lib/state_t.cpp ../lib/state_t.hpp
	$(CC) $(CFLAGS) -c ../lib/state_t.cpp -o ../lib/state_t.o

run: all
	./othello.out

clean:
	rm *.o; rm *.out
