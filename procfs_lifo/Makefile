CXX      := g++
CXXFLAGS := -O3 -std=c++20 -march=native -Wall -Wextra -Wpedantic
LDLIBS   := -lbenchmark -lpthread

BENCH_SRC := procfs_lifo_bmark.cpp
BENCH_BIN := bench.out

KERNEL_DIR := kernel
KDIR := /lib/modules/$(shell uname -r)/build

.PHONY: all bench kmod clean clean-bench clean-kmod

all: bench kmod

bench: $(BENCH_BIN)

$(BENCH_BIN): $(BENCH_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

kmod:
	$(MAKE) -C $(KDIR) M=$(shell pwd)/$(KERNEL_DIR) modules

clean: clean-bench clean-kmod

clean-bench:
	rm -f $(BENCH_BIN)

clean-kmod:
	$(MAKE) -C $(KDIR) M=$(shell pwd)/$(KERNEL_DIR) clean

