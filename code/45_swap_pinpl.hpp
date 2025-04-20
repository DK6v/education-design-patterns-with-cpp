#pragma once

#include <memory>

namespace app {

class WidgetImpl;

class Widget {
public:

    Widget(const int& a);
    Widget(int&& a);

    ~Widget();

    // Copy constructor
    Widget(const Widget& rhs);
    Widget& operator=(const Widget& rhs);

    // Move constructor
    Widget(Widget&& rhs);
    Widget& operator=(Widget&& rhs);

    void swap(Widget& rhs) noexcept;

    void name();

    operator int();
    Widget& operator++();
    Widget operator++(int);

private:
    friend void swap_internal(Widget& lhs, Widget& rhs) noexcept;

private:
    std::unique_ptr<WidgetImpl> pImpl_;   
};

void swap(Widget& lhs, Widget& rhs) noexcept;

} // namespace app

namespace std {
    void swap(app::Widget& lhs, app::Widget& rhs) noexcept;
}