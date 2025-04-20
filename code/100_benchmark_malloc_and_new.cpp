#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include "common.h"

#define REPEAT2(x)  x; x;
#define REPEAT4(x)  REPEAT2(x) REPEAT2(x)
#define REPEAT8(x)  REPEAT4(x) REPEAT4(x)
#define REPEAT16(x) REPEAT8(x) REPEAT8(x)
#define REPEAT32(x) REPEAT16(x) REPEAT16(x)

void BM_NewDelete(benchmark::State& state) {
    const size_t S = state.range(0);
    const size_t N = state.range(0);
    std::vector<char*> v(N);
    for (size_t ix = 0; ix < N; ++ix) { v[ix] = new char[S]; }
    for (auto _ : state) {
        REPEAT32({
            char* p = new char[S];
            benchmark::DoNotOptimize(p);
            delete [] p;
        });
    }
    state.SetItemsProcessed(32 * state.iterations());
    for (size_t ix = 0; ix < N; ++ix) { delete [] v[ix]; }}

void BM_MallocFree(benchmark::State& state) {
    const size_t S = state.range(0);
    const size_t N = state.range(0);
    std::vector<void*> v(N);
    for (size_t ix = 0; ix < N; ++ix) { v[ix] = malloc(S); }
    for (auto _ : state) {
        REPEAT32({
            void* p = malloc(S);
            benchmark::DoNotOptimize(p);
            free(p);
        });
    }
    state.SetItemsProcessed(32 * state.iterations());
    for (size_t ix = 0; ix < N; ++ix) { free(v[ix]); }
}

BENCHMARK(BM_NewDelete)->RangeMultiplier(2)
                       ->Ranges({{32, 256}, {1<<15, 1<<15}})
                       ->ThreadRange(1, 2);

BENCHMARK(BM_MallocFree)->RangeMultiplier(2)
                        ->Ranges({{32, 256}, {1<<15, 1<<15}})
                        ->ThreadRange(1, 2);

BENCHMARK_MAIN();