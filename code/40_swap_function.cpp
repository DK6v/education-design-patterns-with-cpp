#include <assert.h>

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <memory>
#include <type_traits>
#include <cstring>

#include "common.h"

using namespace std;

class Widget {
public:
    Widget() = default;
    Widget(int a) : a_(a) {
        b_.reserve(1000);
        cout << "Widget: constructor" << endl;
    }

    ~Widget() {
        cout << "Widget: desctructor" << endl;
    }

    Widget(const Widget& rhs) {
        cout << "Widget: copy constructor" << endl;
        a_ = rhs.a_;
        b_ = rhs.b_;
        for (int i = 0; i < 1000; ++i) { c_[i] = rhs.c_[i]; }
    }

    Widget& operator=(const Widget& rhs) {
        cout << "Widget: copy operator" << endl;
        a_ = rhs.a_;
        b_ = rhs.b_;

        for (int i = 0; i < 1000; ++i) { c_[i] = rhs.c_[i]; }
        return *this;
    }

    Widget(Widget&& rhs) {
        cout << "Widget: move constructor" << endl;
        a_ = std::move(rhs.a_);
        b_ = std::move(rhs.b_);
        std::memcpy(c_, rhs.c_, sizeof(c_));
    }

    Widget& operator=(Widget&& rhs) {
        cout << "Widget: move operator" << endl;
        a_ = std::move(rhs.a_);
        b_ = std::move(rhs.b_);
        std::memcpy(c_, rhs.c_, sizeof(c_));
        return *this;
    }

    virtual void name() {
        cout << "Widget: " << typeid(decltype(this)).name() << " "
             << hex << a_ << dec << endl;
    }

private:
    int a_;
    vector<int> b_;
    int c_[1000];
};

template <typename T>
void swap_1(T& a, T& b) {
    T tmp(a);
    a = b;
    b = tmp;
}

template <typename T>
void swap_2(T& a, T& b) {
    T tmp(std::move(a));
    a = std::move(b);
    b = std::move(tmp);
}

int main(int argc, char** argv) {

    (void)argc;
    (void)argv;

    cout << "int sz: " << sizeof(int) << endl;
    cout << "int* sz: " << sizeof(int*) << endl;

    int N = 100;

    auto w1 = Widget(1);
    auto w2 = Widget(2);

    cout << "-- create --" << endl;

    w1.name();
    w2.name();

    cout << "-- swap --" << endl;
    swap_1(w1, w2);
    w1.name();
    w2.name();

    cout << "-- swap --" << endl;
    swap_2(w1, w2);
    w1.name();
    w2.name();

    cout << "-- perf 1 --" << endl;

    auto m1 = Measure().start();
    for (int a = 0; a < N; ++a) {
        swap_1(w1, w2);
    }
    m1.stop();
    cout << "duration: " << m1.duration().us() << "us" << endl;

    cout << "-- perf 2 --" << endl;

    auto m2 = Measure().start();
    for (int a = 0; a < N; ++a) {
        swap_2(w1, w2);
    }
    m2.stop();

    cout << "-- perf 3 --" << endl;

    auto m3 = Measure().start();
    for (int a = 0; a < N; ++a) {
        std::swap(w1, w2);
    }
    m3.stop();

    cout << "-- perf 1 --" << endl;
    cout << "duration: " << m1.duration().us() << "us" << endl;
    cout << "-- perf 2 --" << endl;
    cout << "duration: " << m2.duration().us() << "us" << endl;
    cout << "-- perf 3 --" << endl;
    cout << "duration: " << m3.duration().us() << "us" << endl;

    return 0;
}