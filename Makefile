OBJS = bin/ThreadPool.o \

all: bin/executor


bin/executor: src/ThreadPoolManager.cpp $(OBJS)
	g++ -g -W -O2 -std=c++17 -pthread -o $@ $^

bin/%.o: src/%.cpp
	g++ -g -W -O2 -c -pthread -std=c++17 -o $@ $^

clean:
	rm -f bin/*.o bin/executor
