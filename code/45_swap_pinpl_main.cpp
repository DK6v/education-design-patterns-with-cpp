#include "boost/type_index.hpp"
#include "common.h"

#include "45_swap_pinpl.hpp"

// using namespace std;

using Widget = app::Widget;

int main(int argc, char** argv) {

    (void)argc;
    (void)argv;

    int N = 1;

    Widget w = Widget(1);
    auto m = Measure();

    std::cout << "-- Widget().swap(a) " << w << std::endl;
    m.start();
    for (int i = 0; i < N; ++i) {
        Widget(w + 1).swap(w);
    }
    m.stop();
    std::cout << "-- new value: " << w << ", duration: " << m.duration().ms() << "ms" << std::endl;

    std::cout << "-- std::swap(a, b) " << w << std::endl;
    m.start();
    // Compilation error, Widget(1) is rvalue
    // std::swap(Widget(1), a);
    {
        auto tmp = Widget(w + 1);
        std::swap(tmp, w);
    }
    m.stop();
    std::cout << "-- new value: " << w << ", duration: " << m.duration().ms() << "ms" << std::endl;

    std::cout << "-- std::swap(a, b) " << w << std::endl;
    m.start();
    {
        auto tmp = Widget(w);

        std::cout << "   get address: a=" << std::hex << &w << std::dec
             << ", tmp=" << std::hex << &tmp << std::dec << std::endl;

        std::cout << "   copy widget: w -> tmp, w: " << w << ", tmp: " << tmp << std::endl;

        tmp++;
        std::cout << "   modyfy widget: tmp++, w: " << w << ", tmp: " << tmp << std::endl;

        ++tmp;
        std::cout << "   modyfy widget: ++tmp, w: " << w << ", tmp: " << tmp << std::endl;

        std::swap(tmp, w);
        std::cout << "   apply changes: tmp -> a, w: " << w << ", tmp: " << tmp << std::endl;
    }
    m.stop();
    std::cout << "-- new value: " << w << ", duration: " << m.duration().ms() << "ms" << std::endl;

    std::cout << "== app::swap(a, b) " << w << std::endl;
    m.start();
    {
        auto tmp = Widget(w);
        std::cout << "   copy widget: w -> tmp, w: " << w << ", tmp: " << tmp << std::endl;

        // Argument-Dependent Lookup (ADL)
        // Used the app namespace, even it's not specified
        swap(tmp, w);
        std::cout << "   apply changes: tmp -> a, w: " << w << ", tmp: " << tmp << std::endl;
    }
    m.stop();
    std::cout << "-- new value: " << w << ", duration: " << m.duration().ms() << "ms" << std::endl;

    std::cout << "== Widget::swap_int(a, b) " << w << std::endl;
    m.start();
    {
        auto tmp = Widget(w);
        std::cout << "   copy widget: w -> tmp, w: " << w << ", tmp: " << tmp << std::endl;

        swap_internal(tmp, w);
        std::cout << "   apply changes: tmp -> a, w: " << w << ", tmp: " << tmp << std::endl;
    }
    m.stop();
    std::cout << "-- new value: " << w << ", duration: " << m.duration().ms() << "ms" << std::endl;

    std::cout << "== app::swap(a, b) " << w << std::endl;
    m.start();
    {
        auto tmp = Widget(w);
        std::cout << "   copy widget: w -> tmp, w: " << w << ", tmp: " << tmp << std::endl;

        // Never use std::swap()
        //   -> using std::swap; swap();
        //   -> overload swap() in the namespace std, like here.
        swap(tmp, w);
        std::cout << "   apply changes: tmp -> a, w: " << w << ", tmp: " << tmp << std::endl;
    }
    m.stop();
    std::cout << "-- new value: " << w << ", duration: " << m.duration().ms() << "ms" << std::endl;

    return 0;
}