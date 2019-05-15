CC = g++
CFLAGS = -Wall -Wextra -march=native -fPIC -lm -std=c++17 -lstdc++ -DMETRICS -Ofast

serial: benchmark

parallel: CFLAGS += -fopenmp -DPARALLEL
parallel: benchmark

benchmark: benchmark.cpp ai state_t eval_hashmap
	$(CC) $(CFLAGS) benchmark.cpp ../ai/ai.o ../lib/state_t.o ../lib/eval_hashmap.o -o benchmark.out

ai: ../ai/ai.cpp ../ai/ai.hpp state_t eval_hashmap
	$(CC) $(CFLAGS) -c ../ai/ai.cpp -o ../ai/ai.o

state_t: ../lib/state_t.cpp ../lib/state_t.hpp
	$(CC) $(CFLAGS) -c ../lib/state_t.cpp -o ../lib/state_t.o

eval_hashmap: ../lib/eval_hashmap.cpp ../lib/eval_hashmap.hpp
	$(CC) $(CFLAGS) -c ../lib/eval_hashmap.cpp -o ../lib/eval_hashmap.o

run: all
	./benchmark.out

clean:
	rm ../**/*.o; rm ../**/*.out
