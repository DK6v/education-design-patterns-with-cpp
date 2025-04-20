#include "boost/type_index.hpp"

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#define REPEAT2(x)  x; x;
#define REPEAT4(x)  REPEAT2(x) REPEAT2(x)
#define REPEAT8(x)  REPEAT4(x) REPEAT4(x)
#define REPEAT16(x) REPEAT8(x) REPEAT8(x)
#define REPEAT32(x) REPEAT16(x) REPEAT16(x)

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    int & get() { return count_; }

    void LogInfo(std::string message) {
        std::lock_guard<std::mutex> guard(lock_);
        std::cout << "-- Info: " << message << std::endl;
    }

private:
    Logger() = default;

    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    std::mutex lock_;
    int count_;
};

void BM_CreateThenUse(benchmark::State& state) {
    auto & logger = Logger::getInstance();
    for (auto _ : state) {
        REPEAT32(benchmark::DoNotOptimize(++logger.get()));
    }
    state.SetItemsProcessed(32 * state.iterations());
}

void BM_CreateOnUse(benchmark::State& state) {
    for (auto _ : state) {
        REPEAT32(benchmark::DoNotOptimize(++Logger::getInstance().get()));
    }
    state.SetItemsProcessed(32 * state.iterations());
}

BENCHMARK(BM_CreateThenUse);
BENCHMARK(BM_CreateOnUse);

BENCHMARK_MAIN();
