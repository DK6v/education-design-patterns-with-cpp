#include "boost/type_index.hpp"

#include "common.h"

using namespace std;

class Widget {
public:
    Widget() = default;

    Widget(const int& a) : a_(a) {
        cout << "Widget: constructor lvalue, address a="
             << std::hex << &a << std::dec << endl;
    }

    Widget(int&& a) : a_(a) {
        cout << "Widget: constructor rvalue, address a="
             << std::hex << &a << std::dec << endl;
    }

    ~Widget() {
        cout << "Widget: desctructor (" << a_ << ")" << endl;
    }

    Widget(const Widget& rhs) : a_(rhs.a_){
        cout << "Widget: copy constructor, address (lvalue): "

             << std::hex << &rhs << std::dec << endl;
    }

    Widget(Widget&& rhs) : a_(std::move(rhs.a_)) {
        cout << "Widget: move constructor, address (rvalue): "

             << std::hex << &rhs << std::dec << endl;
    }

    Widget& operator=(const Widget& rhs) {
        cout << "Widget: copy operator=, address (lvalue): "

             << std::hex << &rhs << std::dec << endl;
        a_ = rhs.a_;
        return *this;
    }

    Widget& operator=(Widget&& rhs) {
        cout << "Widget: move operator=, address (rvalue): "

             << std::hex << &rhs << std::dec << endl;
        a_ = std::move(rhs.a_);
        return *this;
    }

    void swap(Widget& rhs) noexcept {
        using std::swap;
        swap(*this, rhs);
    }

    virtual void name() {
        cout << "Widget: " << typeid(decltype(this)).name() << " "
             << hex << a_ << dec << endl;
    }

    operator int() {
        return a_;
    }

    Widget operator++(int) {
        auto tmp = *this;
        ++a_;
        return tmp;
    }

    Widget& operator++() {
        ++a_;
        return *this;
    }

private:
    int a_;
};

int main(int argc, char** argv) {

    (void)argc;
    (void)argv;

    int N = 1;

    // int a = 1;
    Widget w = Widget(1);
    auto m = Measure();

    cout << "-- Widget().swap(a) " << w << endl;
    m.start();
    for (int i = 0; i < N; ++i) {
        Widget(w + 1).swap(w);
    }
    m.stop();
    cout << "-- new value: " << w << ", duration: " << m.duration().ms() << "ms" << endl;

    cout << "-- std::swap(a, b) " << w << endl;
    m.start();
    // Compilation error, Widget(1) is rvalue
    // std::swap(Widget(1), a);
    {
        auto tmp = Widget(w + 1);
        std::swap(tmp, w);
    }
    m.stop();
    cout << "-- new value: " << w << ", duration: " << m.duration().ms() << "ms" << endl;

    cout << "-- std::swap(a, b) " << w << endl;
    m.start();
    {
        auto tmp = Widget(w);

        cout << "   get address: a=" << std::hex << &w << std::dec
             << ", tmp=" << std::hex << &tmp << std::dec << endl;

        cout << "   copy widget: a -> tmp, a: " << w << ", tmp: " << tmp << endl;

        tmp++;
        cout << "   modyfy widget: tmp++, a: " << w << ", tmp: " << tmp << endl;

        ++tmp;
        cout << "   modyfy widget: ++tmp, a: " << w << ", tmp: " << tmp << endl;

        std::swap(tmp, w);
        cout << "   apply changes: tmp -> a, a: " << w << ", tmp: " << tmp << endl;
    }
    m.stop();
    cout << "-- new value: " << w << ", duration: " << m.duration().ms() << "ms" << endl;

    return 0;
}