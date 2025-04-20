#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#define REPEAT2(x)  x; x;
#define REPEAT4(x)  REPEAT2(x) REPEAT2(x)
#define REPEAT8(x)  REPEAT4(x) REPEAT4(x)
#define REPEAT16(x) REPEAT8(x) REPEAT8(x)
#define REPEAT32(x) REPEAT16(x) REPEAT16(x)

int foo(int a) { static int i; return a + ++i; }
int bar(int a) { static int i; return a + ++i; }

class A {
public:
    A() : i_(0) {}
    void f(int i) { i_ = foo(i); }
    int get() const { return i_; }
protected:
    int i_;
};

class B {
public:
    B() : i_(0) {}
    virtual ~B() {}
    virtual void f(int i) { i_ = foo(i); }
    virtual int get() const { return i_; }
protected:
    int i_;
};

class C : public B {
    virtual void f(int i) override { i_ = bar(i); }
    virtual int get() const override { return i_; }
};

// Curiously Recursive Template Pattern

template <typename E>
class D {
public:
    D() : i_(0) {}
    void f(int i) { static_cast<E*>(this)->f(i); }
    int get() const { return i_; }
protected:
    int i_;
};

class E : public D<E> {
public:
    void f(int i) { i_ = foo(i); }
};

void BM_NonVirtualFunction(benchmark::State& state) {
    A* a = new A();
    int i = 0;
    for (auto _ : state) {
        REPEAT32(a->f(++i));
    }
    benchmark::DoNotOptimize(a->get());
    state.SetItemsProcessed(32 * state.iterations());
    delete a;
}

void BM_VirtualFunction_B(benchmark::State& state) {
    B* b = new B();
    int i = 0;
    for (auto _ : state) {
        REPEAT32(b->f(++i));
    }
    benchmark::DoNotOptimize(b->get());
    state.SetItemsProcessed(32 * state.iterations());
    delete b;
}

void BM_VirtualFunction_C(benchmark::State& state) {
    B* b = reinterpret_cast<B*>(new C());
    int i = 0;
    for (auto _ : state) {
        REPEAT32(b->f(++i));
    }
    benchmark::DoNotOptimize(b->get());
    state.SetItemsProcessed(32 * state.iterations());
    delete b;
}

void BM_CRTP_Pattern(benchmark::State& state) {
    auto d = new E();
    int i = 0;
    for (auto _ : state) {
        REPEAT32(d->f(++i));
    }
    benchmark::DoNotOptimize(d->get());
    state.SetItemsProcessed(32 * state.iterations());
    delete d;
}

BENCHMARK(BM_NonVirtualFunction);
BENCHMARK(BM_VirtualFunction_B);
BENCHMARK(BM_VirtualFunction_C);
BENCHMARK(BM_CRTP_Pattern);

BENCHMARK_MAIN();