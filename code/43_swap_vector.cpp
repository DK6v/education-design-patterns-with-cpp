#include "boost/type_index.hpp"

#include "common.h"

using namespace std;

class Widget {
public:
    Widget() = default;
    Widget(int a) : a_(a) {
        b_.reserve(1000);
        // cout << "Widget: constructor" << endl;
    }

    ~Widget() {
        // cout << "Widget: desctructor" << endl;
    }

    Widget(const Widget& rhs) {
        // cout << "Widget: copy constructor" << endl;
        a_ = rhs.a_;
        b_ = rhs.b_;
        for (int i = 0; i < 1000; ++i) { c_[i] = rhs.c_[i]; }
    }

    Widget& operator=(const Widget& rhs) {
        // cout << "Widget: copy operator" << endl;
        a_ = rhs.a_;
        b_ = rhs.b_;

        for (int i = 0; i < 1000; ++i) { c_[i] = rhs.c_[i]; }
        return *this;
    }

    Widget(Widget&& rhs) {
        // cout << "Widget: move constructor" << endl;
        a_ = std::move(rhs.a_);
        b_ = std::move(rhs.b_);
        std::memcpy(c_, rhs.c_, sizeof(c_));
    }

    Widget& operator=(Widget&& rhs) {
        // cout << "Widget: move operator" << endl;
        a_ = std::move(rhs.a_);
        b_ = std::move(rhs.b_);
        std::memcpy(c_, rhs.c_, sizeof(c_));
        return *this;
    }

    virtual void name() {
        cout << "Widget: " << typeid(decltype(this)).name() << " "
             << hex << a_ << dec << endl;
    }

    operator int() {
        return a_;
    }

private:
    int a_;
    vector<int> b_;
    int c_[1000];
};

template <typename T>
void swap_copy(T& a, T& b) {
    T tmp(a);
    a = b;
    b = tmp;
}

template <typename T>
void swap_move(T&& a, T&& b) {
    auto t(std::move(a));
    a = std::move(b);
    b = std::move(t);
}

template <typename T>
void swap_forward(T&& a, T&& b) {
    auto t(std::forward<T>(a));
    a = std::forward<T>(b);
    b = std::forward<T>(t);
}

int main(int argc, char** argv) {

    (void)argc;
    (void)argv;

    cout << "int sz: " << sizeof(int) << endl;
    cout << "int* sz: " << sizeof(int*) << endl;

    auto v1 = std::vector<Widget>(1000);
    std::fill(v1.begin(), v1.end(), 1);

    auto v2 = std::vector<Widget>(1000);
    std::fill(v2.begin(), v2.end(), 2);

    int N = 101;

    auto m = Measure();
    cout << "-- ::swap_copy(v1, v2) " << v1[0] << v2[0] << " -> ";
    m.start();
    for (int i = 0; i < N; ++i) {
        ::swap_copy(v1, v2);
    }
    m.stop();
    cout << v1[0] << v2[0] << " duration: " << m.duration().ms() << "ms" << endl;

    cout << "-- ::swap_forward(v1, v2) " << v1[0] << v2[0] << " -> ";
    m.start();
    for (int i = 0; i < N; ++i) {
        ::swap_forward(v1, v2);
    }
    m.stop();
    cout << v1[0] << v2[0] << " duration: " << m.duration().ms() << "ms" << endl;

    cout << "-- ::swap_forward(std::move(v1), std::move(v2)) " << v1[0] << v2[0] << " -> ";
    m.start();
    for (int i = 0; i < N; ++i) {
        ::swap_forward(std::move(v1), std::move(v2));
    }
    m.stop();
    cout << v1[0] << v2[0] << " duration: " << m.duration().ms() << "ms" << endl;

    cout << "-- ::swap_move(v1, v2) " << v1[0] << v2[0] << " -> ";
    m.start();
    for (int i = 0; i < N; ++i) {
        ::swap_move(v1, v2);
    }
    m.stop();
    cout << v1[0] << v2[0] << " duration: " << m.duration().ms() << "ms" << endl;

    cout << "-- v1.swap(v2) " << v1[0] << v2[0] << " -> ";
    m.start();
    for (int i = 0; i < N; ++i) {
        v1.swap(v2);
    }
    m.stop();
    cout << v1[0] << v2[0] << " duration: " << m.duration().ms() << "ms" << endl;

    cout << "-- std::swap(v1, v2) " << v1[0] << v2[0] << " -> ";
    m.start();
    for (int i = 0; i < 100; ++i) {
        std::swap(v1, v2);
    }
    m.stop();
    cout << v1[0] << v2[0] << " duration: " << m.duration().ms() << "ms" << endl;

    // **************************

    int&& wa = Widget(3);
    cout << endl;
    cout << "-- int&& wa = Widget(3): "
         << boost::typeindex::type_id_with_cvr<decltype(wa)>().pretty_name()
         << ((std::is_lvalue_reference<decltype(wa)>::value) ? " lvalue" : " rvalue")
         << endl;

    return 0;
}