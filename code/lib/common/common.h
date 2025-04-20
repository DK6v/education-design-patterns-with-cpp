#pragma once

#include <assert.h>

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <memory>
#include <type_traits>
#include <cstring>

#include <chrono>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

class Measure {
public:
    class nanoseconds {
    public:
        nanoseconds(int64_t ns) : ns_(ns) {}
        operator int64_t() { return ns_; }
        inline int64_t ms() { return ns_ / 1'000'000; }
        inline int64_t us() { return ns_ / 1'000; }
        inline int64_t ns() { return ns_; }
    private:
        int64_t ns_;
    };

    const Measure& start();
    const Measure& stop();

    nanoseconds duration();
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
    std::chrono::time_point<std::chrono::high_resolution_clock> stop_;
};

template<typename Т>
class TD;

template <typename T>
constexpr bool is_lvalue(T&) {
    return true;
}

template <typename T>
constexpr bool is_lvalue(T&&) {
    return false;
}
