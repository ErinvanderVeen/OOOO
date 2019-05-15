CC = g++
CFLAGS = -Wall -Wextra -march=native -fPIC -lm -std=c++17 -lstdc++

serial: CFLAGS += -Ofast
serial: interface

serialdebug: CFLAGS += -g -DDEBUG -gdwarf-2
serialdebug: interface

parallel: CFLAGS += -fopenmp -Ofast -DPARALLEL
parallel: interface

paralleldebug: CFLAGS += -fopenmp -g -DPARALLEL -DDEBUG
paralleldebug: interface

# Passing this flag will enable performance metrics
metrics: CFLAGS += -fopenmp -Ofast -DPARALLEL -DMETRICS
metrics: interface

interface: interface.cpp ai state_t eval_hashmap
	$(CC) $(CFLAGS) interface.cpp ai.o ../lib/state_t.o ../lib/eval_hashmap.o -o interface.out

ai: ai.cpp ai.hpp state_t eval_hashmap
	$(CC) $(CFLAGS) -c ai.cpp -o ai.o

state_t: ../lib/state_t.cpp ../lib/state_t.hpp
	$(CC) $(CFLAGS) -c ../lib/state_t.cpp -o ../lib/state_t.o

eval_hashmap: ../lib/eval_hashmap.cpp ../lib/eval_hashmap.hpp
	$(CC) $(CFLAGS) -c ../lib/eval_hashmap.cpp -o ../lib/eval_hashmap.o

run: all
	./interface.out

clean:
	rm ../**/*.o; rm ../**/*.out
