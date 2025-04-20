#include "boost/type_index.hpp"

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include "152_singletone_logger_pimpl.hpp"

#define REPEAT2(x)  x; x;
#define REPEAT4(x)  REPEAT2(x) REPEAT2(x)
#define REPEAT8(x)  REPEAT4(x) REPEAT4(x)
#define REPEAT16(x) REPEAT8(x) REPEAT8(x)
#define REPEAT32(x) REPEAT16(x) REPEAT16(x)

void BM_CreateThenUse(benchmark::State& state) {
    Logger logger;
    for (auto _ : state) {
        REPEAT32(benchmark::DoNotOptimize(++logger.get()));
    }
    state.SetItemsProcessed(32 * state.iterations());
}

void BM_CreateOnUse(benchmark::State& state) {
    for (auto _ : state) {
        REPEAT32(benchmark::DoNotOptimize(++Logger().get()));
    }
    state.SetItemsProcessed(32 * state.iterations());
}

BENCHMARK(BM_CreateThenUse);
BENCHMARK(BM_CreateOnUse);

BENCHMARK_MAIN();
