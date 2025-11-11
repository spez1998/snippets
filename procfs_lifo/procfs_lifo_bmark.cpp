#include <memory>
#include <benchmark/benchmark.h>

constexpr auto device_path{"/proc/sm/procfs_lifo"};
constexpr auto random_source{"/dev/urandom"};

/* Test plan:
 * 1. Writes. For SIZE in SIZES:
 *   a) Load device file
 *   b) Create temporary buffer SIZE large, fill with /dev/urandom data
 *   c) Write temp buffer's data into LIFO - TIMED
 *   d) Unload device file
 *
 * 2. Reads. For SIZE in SIZES:
 *   a) Load device file
 *   b) Create temporary buffer SIZE large, fill with /dev/urandom data
 *   c) Write temp buffer's data into LIFO
 *   d) Read back all written data - TIMED
 *   e) Unload device file
 */

void LoadModule() {
	std::system("insmod kernel/procfs_lifo.ko");
}

void UnloadModule() {
	std::system("rmmod procfs_lifo");
}

bool FillBufRandom() {
	return true;
}

void WriteToLIFO() {
	;
}

void ReadFromLIFO() {
	;
}

/*
 * static void DoSetup(const benchmark::State& state) {
 *     LoadModule();
 *     FillBufRandom();
 * }
 *
 * static void DoTeardown(...) {
 *     UnloadModule();
 * }
 *
 * static void WriteLIFO() {
 * }
 *
 * static void BM_Write(...) {
 *     WriteLIFO();
 * }
 *
 * static void BM_Read(...) {
 *     ...
 * }
