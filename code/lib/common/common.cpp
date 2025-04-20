#include <chrono>

#include "common.h"

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

const Measure& Measure::start() {
    start_ = std::chrono::high_resolution_clock::now();
    return *this;
}

const Measure& Measure::stop() {
    stop_ = std::chrono::high_resolution_clock::now();
    return *this;
}

Measure::nanoseconds Measure::duration() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(stop_ - start_).count();
}
