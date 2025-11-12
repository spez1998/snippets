#include <array>
#include <benchmark/benchmark.h>
#include <fcntl.h>
#include <memory>
#include <random>
#include <stdexcept>
#include <unistd.h>

template <typename T, size_t S>
class ProcfsLifoTester : public benchmark::Fixture {
  public:
    void SetUp(const benchmark::State &state) override {
        bool prewrite{state.range(0) == 1 ? true : false};
        std::system("insmod kernel/procfs_lifo.ko");
        if ((fd_ = open("/proc/suj/procfs_lifo", O_RDWR)) < 0) {
			std::system("rmmod procfs_lifo");
            throw std::runtime_error("Couldn't open proc file");
        }

        userbuf_ = std::make_unique<std::array<T, S>>();
        if (prewrite) {
            FillUserbufRandom();
            WriteLifo();
        }
    }

    void TearDown(const benchmark::State &state) override {
        close(fd_);
        std::system("rmmod procfs_lifo");
    }

    void WriteLifo() { write(fd_, userbuf_->data(), S); }
    void ReadLifo() { read(fd_, userbuf_->data(), S); }

  private:
    void FillUserbufRandom() {
        std::mt19937 mt{std::random_device{}()};
        std::uniform_int_distribution dst{0, 255};
        for (size_t i{0}; i < S; ++i) {
            userbuf_.get()->at(i) = dst(mt);
        }
    }

    int fd_;
    std::unique_ptr<std::array<T, S>> userbuf_;
};

using ProcfsLifoTester_u8_10k = ProcfsLifoTester<uint8_t, 10000>;

BENCHMARK_F(ProcfsLifoTester_u8_10k, WriteBench)(benchmark::State &state) {
	for (auto _ : state) {
		this->WriteLifo();
		benchmark::ClobberMemory();
	}
}

BENCHMARK_REGISTER_F(ProcfsLifoTester_u8_10k, WriteBench)->Arg(0)->ThreadRange(1, 1);
BENCHMARK_MAIN();
