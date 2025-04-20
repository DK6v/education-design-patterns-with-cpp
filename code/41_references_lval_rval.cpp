#include <assert.h>

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <memory>
#include <type_traits>
#include <cstring>

#include "boost/type_index.hpp"

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
void foo(T&& w) {
    cout << "foo: "
         << boost::typeindex::type_id_runtime(w).pretty_name()
         << ((std::is_lvalue_reference<T>::value) ? " lvalue" : " rvalue")
         << endl;

}

// NOT working!!!
// Overloading a universal reference is not used, such a function is the greediest.
// See above.
template <typename T>
void foo(const T&& w) {
    cout << "foo: CONST "
         << boost::typeindex::type_id_runtime(w).pretty_name()
         << ((std::is_lvalue_reference<T>::value) ? " lvalue" : " rvalue")
         << endl;

}

int main(int argc, char** argv) {

    (void)argc;
    (void)argv;

    cout << "int sz: " << sizeof(int) << endl;
    cout << "int* sz: " << sizeof(int*) << endl;

    auto w1 = Widget(1);

    cout << "-- create --" << endl;
    w1.name();

    cout << "-- foo variable --" << endl;
    foo(w1);

    auto & w2 = w1;
    cout << "-- foo reference --" << endl;
    foo(w2);

    cout << "-- foo rvalue --" << endl;
    foo(Widget(2));

    const auto & w3 = w1;
    cout << "-- foo const reference --" << endl;
    foo(w3);

    const Widget w4 = w1;
    cout << "-- foo const variable --" << endl;
    foo(w3);

    return 0;
}