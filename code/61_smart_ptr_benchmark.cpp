#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include "common.h"

template <typename T>
class SmartPtr {
public:
    template <typename Deleter>
    SmartPtr(T* p, Deleter d) : p_(p), d_(new deleter<Deleter>(d)) {}
    ~SmartPtr() { d_->apply(p_); delete d_; }

    T* operator*() { return p_; }

private:
    struct deleter_base {
        virtual void apply(void*) = 0;
        virtual ~deleter_base() = default;
    };

    template <typename Deleter>
    struct deleter : public deleter_base {
        deleter(Deleter d) : d_(d) {}
        virtual void apply(void* p) { d_(static_cast<T*>(p)); }

        Deleter d_;
    };

private:
    T* p_;
    deleter_base* d_;
};

struct Deleter {
    template<typename T>
    void operator() (T* ptr) { delete ptr; }
};

void BM_RawPtr(benchmark::State& state) {
    Deleter deleter;
    for (auto _ : state) {
        int* p = new int(123);
        benchmark::DoNotOptimize(*p);
        deleter(p);
    }
    state.SetItemsProcessed(state.iterations());
}

void BM_UniquePtr(benchmark::State& state) {
    Deleter deleter;
    for (auto _ : state) {
        std::unique_ptr<int, Deleter> p(new int(123), deleter);
        benchmark::DoNotOptimize(*p);
    }
    state.SetItemsProcessed(state.iterations());
}

void BM_SharedPtr(benchmark::State& state) {
    Deleter deleter;
    for (auto _ : state) {
        std::shared_ptr<int> p(new int(123), deleter);
        benchmark::DoNotOptimize(*p);
    }
    state.SetItemsProcessed(state.iterations());
}

void BM_SmartPtr(benchmark::State& state) {
    Deleter deleter;
    for (auto _ : state) {
        SmartPtr<int> p(new int(123), deleter);
        benchmark::DoNotOptimize(*p);
    }
    state.SetItemsProcessed(state.iterations());
}

BENCHMARK(BM_RawPtr);
BENCHMARK(BM_UniquePtr);
BENCHMARK(BM_SharedPtr);
BENCHMARK(BM_SmartPtr);
BENCHMARK_MAIN();