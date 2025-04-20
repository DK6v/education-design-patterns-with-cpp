#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include "common.h"

#define REPEAT2(x)  x; x;
#define REPEAT4(x)  REPEAT2(x) REPEAT2(x)
#define REPEAT8(x)  REPEAT4(x) REPEAT4(x)
#define REPEAT16(x) REPEAT8(x) REPEAT8(x)
#define REPEAT32(x) REPEAT16(x) REPEAT16(x)

template <typename T>
struct ScopedPtr {
    T* ptr;
    ScopedPtr(T* ptr) : ptr(ptr) {}
    ~ScopedPtr() { delete ptr; }
    T* operator*() { return ptr; }
};

void BM_RawPtrDereference(benchmark::State& state) {
    int* p = new int;
    for (auto _ : state) {
        REPEAT32(benchmark::DoNotOptimize(*p));
    }
    state.SetItemsProcessed(32 * state.iterations());
}

void BM_ScopedPtrDereference(benchmark::State& state) {
    ScopedPtr p = new int;
    for (auto _ : state) {
        REPEAT32(benchmark::DoNotOptimize(*p));
    }
    state.SetItemsProcessed(32 * state.iterations());
}

void BM_UniquePtrDereference(benchmark::State& state) {
    std::unique_ptr<int> p = std::make_unique<int>();
    for (auto _ : state) {
        REPEAT32(benchmark::DoNotOptimize(*p));
    }
    state.SetItemsProcessed(32 * state.iterations());
}

BENCHMARK(BM_RawPtrDereference);
BENCHMARK(BM_ScopedPtrDereference);
BENCHMARK(BM_UniquePtrDereference);

BENCHMARK_MAIN();