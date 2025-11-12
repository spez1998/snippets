# ----- Compiler and Flags -----
CXX      := g++
CXXFLAGS := -O3 -std=c++20 -march=native -Wall -Wextra -Wpedantic

# If you want debug instead of release, comment above and uncomment:
# CXXFLAGS := -g -std=c++20 -Wall -Wextra -Wpedantic

# ----- Google Benchmark / pthread -----
# These must be linked manually:
LDLIBS := -lbenchmark -lpthread

# ----- Source and Output -----
SRC := procfs_lifo_bmark.cpp
BIN := bench

# ----- Build Rule -----
$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

# ----- Cleanup -----
clean:
	rm -f $(BIN)

.PHONY: clean

