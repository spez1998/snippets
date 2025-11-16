#include <benchmark/benchmark.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <array>
#include <chrono>
#include <memory>
#include <random>
#include <stdexcept>
#include <thread>

#include "kernel/procfs_lifo_meminfo.h"

template <typename T, size_t S>
class ProcfsLifoTester : public benchmark::Fixture {
   public:
    void SetUp(const benchmark::State& state) override {
        bool prewrite{state.range(0) == 1 ? true : false};
        if ((fd_ = open("/proc/suj/procfs_lifo", O_RDWR)) < 0) {
            throw std::runtime_error("Couldn't open proc file");
        }

        ClearLifo();
        std::system("sudo su -c 'echo 1 > /proc/sys/vm/drop_caches'");
        std::system("sudo su -c 'echo 1 > /proc/sys/vm/compact_memory'");

        userbuf_ = std::make_unique<std::array<T, S>>();
        if (prewrite) {
            FillUserbufRandom();
            WriteLifo();
        }
    }

    void TearDown(const benchmark::State& state) override { close(fd_); }

    void WriteLifo() { write(fd_, userbuf_->data(), S * sizeof(T)); }
    void ReadLifo() { read(fd_, userbuf_->data(), S * sizeof(T)); }

   private:
    void FillUserbufRandom() {
        std::mt19937 mt{std::random_device{}()};
        std::uniform_int_distribution<T> dst{0, 255};
        for (size_t i{0}; i < S; ++i) {
            (*userbuf_)[i] = dst(mt);
        }
    }

    void ClearLifo() { ::ioctl(fd_, CLEAR_LIFO); }

    int fd_;
    std::unique_ptr<std::array<T, S>> userbuf_;
};

BENCHMARK_TEMPLATE_DEFINE_F(ProcfsLifoTester, WriteBench10, uint8_t, 10)(benchmark::State& state) {
    for (auto _ : state) {
        this->WriteLifo();
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(ProcfsLifoTester, WriteBench10)->Arg(0)->ThreadRange(1, 1);

BENCHMARK_TEMPLATE_DEFINE_F(ProcfsLifoTester, WriteBench100, uint8_t, 100)(benchmark::State& state) {
    for (auto _ : state) {
        this->WriteLifo();
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(ProcfsLifoTester, WriteBench100)->Arg(0)->ThreadRange(1, 1);

BENCHMARK_TEMPLATE_DEFINE_F(ProcfsLifoTester, WriteBench1_000, uint8_t, 1'000)(benchmark::State& state) {
    for (auto _ : state) {
        this->WriteLifo();
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(ProcfsLifoTester, WriteBench1_000)->Arg(0)->ThreadRange(1, 1);

BENCHMARK_TEMPLATE_DEFINE_F(ProcfsLifoTester, WriteBench10_000, uint8_t, 10'000)(benchmark::State& state) {
    for (auto _ : state) {
        this->WriteLifo();
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(ProcfsLifoTester, WriteBench10_000)->Arg(0)->ThreadRange(1, 1);

BENCHMARK_TEMPLATE_DEFINE_F(ProcfsLifoTester, WriteBench100_000, uint8_t, 100'000)(benchmark::State& state) {
    for (auto _ : state) {
        this->WriteLifo();
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(ProcfsLifoTester, WriteBench100_000)->Arg(0)->ThreadRange(1, 1);

BENCHMARK_TEMPLATE_DEFINE_F(ProcfsLifoTester, WriteBench1_000_000, uint8_t, 1'000'000)(benchmark::State& state) {
    for (auto _ : state) {
        this->WriteLifo();
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(ProcfsLifoTester, WriteBench1_000_000)->Arg(0)->ThreadRange(1, 1);

BENCHMARK_MAIN();
