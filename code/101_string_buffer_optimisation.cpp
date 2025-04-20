#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include "common.h"

#define REPEAT2(x)  x; x;
#define REPEAT4(x)  REPEAT2(x) REPEAT2(x)
#define REPEAT8(x)  REPEAT4(x) REPEAT4(x)
#define REPEAT16(x) REPEAT8(x) REPEAT8(x)
#define REPEAT32(x) REPEAT16(x) REPEAT16(x)

const char gTestString[] =
    "0---------10--------20--------30--------40--------50--------60--------70--------80--------90--------";

class SimpleString {
public:
    SimpleString() : s_(nullptr) {
        // std::cout << "default constructor" << std::endl;
    }
    SimpleString(const SimpleString& s) : s_(strdup(s.s_)) {
        // std::cout << "copy constructor" << std::endl;
    }

    explicit SimpleString(const char* s) : s_(strdup(s)) {
        // std::cout << "explicit constructor (const char*)" << std::endl;
    }

    SimpleString& operator=(const char* s) {
        // std::cout << "operator= (char*)" << std::endl;
        free(s_);
        s_ = strdup(s);
        return *this;
    }

    SimpleString(SimpleString&& other) {
        // std::cout << "move constructor" << std::endl;
        *this = other;
    };

    SimpleString& operator=(SimpleString&& other) {
        // std::cout << "move operator" << std::endl;
        s_ = other.s_;
        other.s_ = nullptr;
        return *this;
    };

    SimpleString& operator=(const SimpleString& s) {
        // std::cout << "operator= (SimpleStrings&)" << std::endl;
        free(s_);
        s_ = strdup(s.s_);
        return *this;
    }

    ~SimpleString() {
        // std::cout << "destructor, free" << std::endl;
        free(s_);
    }

    const char* operator*() const { return s_; }

    bool operator==(const SimpleString& rhs) const {
        return strcmp(s_, rhs.s_) == 0;
    }

private:
    char* s_;
};

class SmartString {
public:
    SmartString() {
        b_[0] = 0;
        tag_.value = Tag::BUFFER;
        // std::cout << "default constructor" << std::endl;
    }

    SmartString(const SmartString& other) {
        // std::cout << "constructor (const SmartString&)" << std::endl;
        duplicate(other);
    }

    SmartString& operator=(const SmartString& other) {
        // std::cout << "operator= (const SmartString&)" << std::endl;
        if (tag_.value == Tag::HEAP && s_ != nullptr) {

            // std::cout << "free" << std::endl;
            free(s_);
        }
        duplicate(other);
        return *this;
    }

    explicit SmartString(const char* s) {
        // std::cout << "constructor (const char*)" << std::endl;
        duplicate(s);
    }

    SmartString& operator=(const char* s) {
        // std::cout << "operator= (const char*)" << std::endl;
        if (tag_.value == Tag::HEAP && s_ != nullptr) { free(s_); }
        duplicate(s);
        return *this;
    }

    SmartString(SmartString&& other) {
        // std::cout << "move constructor" << std::endl;
        *this = other;
    };

    SmartString& operator=(SmartString&& other) {
        // std::cout << "move operator" << std::endl;
        memcpy(b_, other.b_, sizeof(b_));
        other.tag_.value = Tag::BUFFER;
        return *this;
    };

    ~SmartString() {
        // std::cout << "destructor" << std::endl;
        if (tag_.value == Tag::HEAP && s_ != nullptr) {
            // std::cout << "free" << std::endl;
            free(s_);
        }
    }

    const char* operator*() const {
        return s_;
    }

    bool operator==(const SmartString& rhs) const {
        if (tag_.value == Tag::HEAP && s_ == nullptr) return false;
        if (rhs.tag_.value == Tag::HEAP && rhs.s_ == nullptr) return false;
        return strcmp((tag_.value == Tag::BUFFER) ? b_ : s_,
                      (rhs.tag_.value == Tag::BUFFER) ? rhs.b_ : rhs.s_) == 0;
    }

private:
    inline void duplicate(const char* s) {
        if (strlen(s) < sizeof(b_)) {
            tag_.value = Tag::BUFFER;

            std::strncpy(b_, s, sizeof(b_));
        }
        else {
            tag_.value = Tag::HEAP;
            s_ = strdup(s);
        }
    }

    inline void duplicate(const SmartString& other) {
        tag_.value = other.tag_.value;
        if (other.tag_.value == Tag::BUFFER) {
            std::memcpy(b_, other.b_, sizeof(b_));
        }
        else {
            s_ = strdup(other.s_);
        }
    }

    enum class Tag: char { BUFFER = 0, HEAP };
    union {
        char* s_;
        char b_[16];
        struct {
            char _[sizeof(b_) - 1];
            Tag value;
        } tag_;
    };
};

template <typename T>
void BM_LocalVariable(benchmark::State& state) {
    const size_t S = state.range(0);
    auto s = std::shared_ptr<char[]>(new char[S], std::default_delete<char[]>());
    snprintf(s.get(), S, "%s", gTestString);
    for (auto _ : state) {
        auto out = T(s.get());
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(state.iterations());
}

template <typename T>
void BM_HeapVariable(benchmark::State& state) {
    const size_t S = state.range(0);
    auto s = std::shared_ptr<char[]>(new char[S], std::default_delete<char[]>());
    snprintf(s.get(), S, "%s", gTestString);
    for (auto _ : state) {
        auto out = new T(s.get());
        benchmark::DoNotOptimize(*out);
    }
    state.SetItemsProcessed(state.iterations());
}

template <typename T>
void BM_CompareVectorCopy(benchmark::State& state) {
    const size_t S = state.range(0);
    const size_t N = state.range(1);

    auto s = std::shared_ptr<char[]>(new char[S], std::default_delete<char[]>());
    snprintf(s.get(), S, "%s", gTestString);

    std::vector<T> v1(N), v2(N);
    for (size_t n = 0; n < N; ++n) {
        v1[n] = T(s.get());
        v2[n] = T(s.get());
    }

    for (auto _ : state) {
        for (size_t n = 0; n < N; ++n) {
            benchmark::DoNotOptimize(v1[n] == v2[n]);
        }
    }
    state.SetItemsProcessed(state.iterations());
}

template <typename T>
void BM_CompareVectorMove(benchmark::State& state) {
    const size_t S = state.range(0);
    const size_t N = state.range(1);

    auto s = std::shared_ptr<char[]>(new char[S], std::default_delete<char[]>());
    snprintf(s.get(), S, "%s", gTestString);

    std::vector<T> v1, v2;
    for (size_t n = 0; n < N; ++n) {
        v1.emplace_back(s.get());
        v2.emplace_back(s.get());
    }

    for (auto _ : state) {
        for (size_t n = 0; n < N; ++n) {
            benchmark::DoNotOptimize(v1[n] == v2[n]);
        }
    }
    state.SetItemsProcessed(state.iterations());
}

// BENCHMARK(BM_LocalVariable<SimpleString>)->Arg(8);
// BENCHMARK(BM_LocalVariable<SmartString>)->Arg(8);
//

// BENCHMARK(BM_LocalVariable<SimpleString>)->Arg(64);
// BENCHMARK(BM_LocalVariable<SmartString>)->Arg(64);

// BENCHMARK(BM_HeapVariable<SimpleString>)->Arg(8);
// BENCHMARK(BM_HeapVariable<SmartString>)->Arg(8);
//

// BENCHMARK(BM_HeapVariable<SimpleString>)->Arg(64);
// BENCHMARK(BM_HeapVariable<SmartString>)->Arg(64);

// BENCHMARK(BM_CompareVectorCopy<SimpleString>)->Args({8, 1024*1024});
// BENCHMARK(BM_CompareVectorCopy<SmartString>)->Args({8, 1024*1024});
//

// BENCHMARK(BM_CompareVectorCopy<SimpleString>)->Args({64, 1024*1024});
// BENCHMARK(BM_CompareVectorCopy<SmartString>)->Args({64, 1024*1024});

BENCHMARK(BM_CompareVectorMove<SimpleString>)->Args({8, 1024*1024});
BENCHMARK(BM_CompareVectorMove<SmartString>)->Args({8, 1024*1024});

BENCHMARK(BM_CompareVectorMove<SimpleString>)->Args({64, 1024*1024});
BENCHMARK(BM_CompareVectorMove<SmartString>)->Args({64, 1024*1024});

// --------------------------------------------------------------------------------------------------------
// Benchmark                                              Time             CPU   Iterations UserCounters...
// --------------------------------------------------------------------------------------------------------
// BM_CompareVectorCopy<SimpleString>/8/1048576     4374238 ns      4373547 ns          166 items_per_second=228.647/s
// BM_CompareVectorCopy<SmartString>/8/1048576      3397063 ns      3396906 ns          205 items_per_second=294.385/s
// BM_CompareVectorCopy<SimpleString>/64/1048576    8803903 ns      8803004 ns           80 items_per_second=113.598/s
// BM_CompareVectorCopy<SmartString>/64/1048576     9618396 ns      9617556 ns           72 items_per_second=103.977/s

// --------------------------------------------------------------------------------------------------------
// Benchmark                                              Time             CPU   Iterations UserCounters...
// --------------------------------------------------------------------------------------------------------
// BM_CompareVectorMove<SimpleString>/8/1048576     7395099 ns      7394734 ns           96 items_per_second=135.231/s
// BM_CompareVectorMove<SmartString>/8/1048576      2661553 ns      2661356 ns          263 items_per_second=375.748/s
// BM_CompareVectorMove<SimpleString>/64/1048576   13311299 ns     13310208 ns           52 items_per_second=75.1303/s
// BM_CompareVectorMove<SmartString>/64/1048576    15755078 ns     15753315 ns           43 items_per_second=63.4787/s

BENCHMARK_MAIN();
