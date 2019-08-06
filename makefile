CC = g++
STD = -std=c++17
FLAGS = -lgmp -lgmpxx -pthread
OPTIM = -mtune=native -march=native -flto -O3
PROGRAM = mersenne

$(PROGRAM).out: main.cpp
	$(CC) main.cpp $(STD) $(FLAGS) -o $(PROGRAM).out $(OPTIM)

run:
	time ./$(PROGRAM).out

clean:
	rm -f *.out
