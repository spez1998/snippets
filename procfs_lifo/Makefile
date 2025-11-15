CXX      := g++
CXXFLAGS := -O3 -std=c++20 -march=native -Wall -Wextra -Wpedantic
# CXXFLAGS := -g -std=c++20 -Wall -Wextra -Wpedantic

LDLIBS := -lbenchmark -lpthread

SRC := procfs_lifo_bmark.cpp
BIN := bench.out

$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f $(BIN)

.PHONY: clean

