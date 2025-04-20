#include <assert.h>

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <memory>

using namespace std;

class Widget {
public:
    Widget() {
        cout << "Widget: constructor" << endl;
    }
    ~Widget() {
        cout << "Widget: desctructor" << endl;
    }
    virtual std::string name() {
        return "Widget";
    }
};

class WidgetFactory {
public:
    std::unique_ptr<Widget> create() {
        return std::unique_ptr<Widget>(new Widget);
    }
};

void foo(Widget* ptr) {
    cout << "foo: " << ptr->name() << endl;
}

void bar(shared_ptr<Widget> ptr) {
    cout << "bar: " << ptr->name() << endl;
}

int main(int argc, char** argv) {

    (void)argc;
    (void)argv;

    cout << "int sz: " << sizeof(int) << endl;
    cout << "int* sz: " << sizeof(int*) << endl;

    auto wf = WidgetFactory();
    auto u_ptr = wf.create();

    foo(u_ptr.get());
    foo(&*u_ptr);

    std::shared_ptr<Widget> s_ptr = std::move(u_ptr);

    bar(s_ptr);

    return 0;
}