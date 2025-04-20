#include "boost/type_index.hpp"
#include "common.h"

#include "45_swap_pinpl.hpp"

#include <stdexcept>

namespace app {

class WidgetImpl {
public:
//    WidgetImpl() = default;

    WidgetImpl(const int& a) : a_(a) {
        std::cout << "WidgetImpl: constructor lvalue, address a="
             << std::hex << &a << std::dec << std::endl;
    }

    WidgetImpl(int&& a) : a_(std::move(a)) {
        std::cout << "WidgetImpl: constructor rvalue, address a="
             << std::hex << &a << std::dec << std::endl;
    }

    ~WidgetImpl() {
        std::cout << "WidgetImpl: desctructor (" << a_ << ")" << std::endl;
    }

    WidgetImpl(const WidgetImpl& rhs) : a_(rhs.a_) {
        std::cout << "WidgetImpl: copy constructor, address (lvalue): "

             << std::hex << &rhs << std::dec << std::endl;
    }

    WidgetImpl(WidgetImpl&& rhs) : a_(std::move(rhs.a_)) {
        std::cout << "WidgetImpl: move constructor, address (rvalue): "

             << std::hex << &rhs << std::dec << std::endl;
    }

    WidgetImpl& operator=(const WidgetImpl& rhs) {
        std::cout << "WidgetImpl: copy operator=, address (lvalue): "

             << std::hex << &rhs << std::dec << std::endl;
        a_ = rhs.a_;
        return *this;
    }

    WidgetImpl& operator=(WidgetImpl&& rhs) {
        std::cout << "WidgetImpl: move operator=, address (rvalue): "

             << std::hex << &rhs << std::dec << std::endl;
        a_ = std::move(rhs.a_);
        return *this;
    }

    void swap(WidgetImpl& rhs) noexcept {
        using std::swap;
        swap(*this, rhs);
    }

    virtual void name() {
        std::cout << "WidgetImpl: " << typeid(decltype(this)).name() << " "
             << std::hex << a_ << std::dec << std::endl;
    }

    operator int() {
        return a_;
    }

    WidgetImpl operator++(int) {
        auto tmp = *this;
        ++a_;
        return tmp;
    }

    WidgetImpl& operator++() {
        ++a_;
        return *this;
    }

private:
    int a_;
};

Widget::Widget(const int& a)
: pImpl_(std::make_unique<WidgetImpl>(a)) {}

Widget::Widget(int&& a)
: pImpl_(std::make_unique<WidgetImpl>(std::move(a))) {}

// Defined here for correct deletion of the unique_ptr
Widget::~Widget() = default;

// Copy constructor
Widget::Widget(const Widget& rhs)
: pImpl_(nullptr) {

    if (rhs.pImpl_) {
        //std::cout << "Widget: copy constructor" << std::endl;
        pImpl_ = std::make_unique<WidgetImpl>(*rhs.pImpl_);
    }

}

Widget& Widget::operator=(const Widget& rhs) {
    if (!rhs.pImpl_) {
        std::cout << "Widget: copy operator (reset)" << std::endl;
        pImpl_.reset();
    }
    else if (!pImpl_) {
        std::cout << "Widget: copy operator (create)" << std::endl;
        pImpl_ = std::make_unique<WidgetImpl>(*rhs.pImpl_);
    }
    else {
        std::cout << "Widget: copy operator (copy)" << std::endl;
        *pImpl_ = *rhs.pImpl_;
    }
    return *this;
}

// Move constructor
Widget::Widget(Widget&& rhs)
: pImpl_(std::move(rhs.pImpl_)) {}

Widget& Widget::operator=(Widget&& rhs) {
    pImpl_ = std::move(rhs.pImpl_);
    return *this;
}

void Widget::swap(Widget& rhs) noexcept {
    using std::swap;
    swap(pImpl_, rhs.pImpl_);
}

Widget::operator int() {
    if (pImpl_) {
        return static_cast<int>(*pImpl_);
    }

    throw std::bad_cast();
}

// Prefix increment operator
Widget& Widget::operator++() {
    if (pImpl_) {
        ++(*pImpl_);
        return *this;
    }

    throw std::bad_cast();
}

// Postfix increment operator
Widget Widget::operator++(int) {
    if (pImpl_) {
        auto tmp = *this;
        ++(*pImpl_);
        return tmp;
    }

    throw std::bad_cast();
}

void swap(Widget& lhs, Widget& rhs) noexcept {
    std::cout << "app::swap lhs="
              << std::hex << &lhs << ", rhs=" << &rhs << std::dec << std::endl;
    lhs.swap(rhs);
}

void swap_internal(Widget& lhs, Widget& rhs) noexcept {
    std::cout << "Widget::swap_internal "
              << std::hex << "lhs=" << &lhs << ", rhs=" << &rhs << std::dec
              << std::endl;
    lhs.swap(rhs);
}

} // namespace app

namespace std {
    void swap(app::Widget& lhs, app::Widget& rhs) noexcept {
        std::cout << "std::swap (redefined) "
              << std::hex << "lhs=" << &lhs << ", rhs=" << &rhs << std::dec
              << std::endl;
        lhs.swap(rhs);
    }
}