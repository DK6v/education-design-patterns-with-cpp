#include <assert.h>

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <string>

using namespace std;

void foo(int& a)  { cout << "int& : " << a << endl; }
void foo(int&& a) { cout << "int&& : " << a << endl; }

void bar(int& b) { cout << "int& -> "; foo(b); }
void bar(int&& b) { cout << "int&& -> "; foo(b); }

void bar_f(int& b) { cout << "int& -> "; foo(std::forward<decltype(b)>(b));; }
void bar_f(int&& b) { cout << "int&& -> "; foo(std::forward<decltype(b)>(b)); }

void bar_m(int& b) { cout << "int& -> "; foo(std::move(b));; }
void bar_m(int&& b) { cout << "int&& -> "; foo(std::move(b)); }

int main(int argc, char** argv) {

    (void)argc;
    (void)argv;

    cout << "int sz: " << sizeof(int) << endl;
    cout << "int* sz: " << sizeof(int*) << endl;

    std::cout << "\n1:\n";
    foo(1);
    foo(std::move(1));
    foo(std::forward<int>(1));

    std::cout << "\nint a:\n";
    int a = 1;
    foo(a);
    foo(std::move(a));
    foo(std::forward<decltype(a)>(a));

    cout << "\nint& r:\n";
    int& r = a;
    foo(r);
    foo(std::move(r));
    foo(std::forward<decltype(r)>(r));

    cout << "\nbar():\n";
    bar(a);

    cout << "\nbar() move:\n";
    bar(std::move(a));
    bar(std::move(r));

    cout << "\nbar() forward:\n";
    bar(std::forward<decltype(r)>(a));
    bar(std::forward<decltype(r)>(r));

    cout << "\nbar_forward():\n";
    bar_f(a);
    bar_f(r);

    cout << "\nbar_forward(): move\n";
    bar_f(std::move(a));
    bar_f(std::move(r));

    cout << "\nbar_forward(): move\n";
    bar_f(std::forward<decltype(r)>(a));
    bar_f(std::forward<decltype(r)>(r));

    cout << "\nbar_move():\n";
    bar_m(a);
    bar_m(r);

    cout << "\nbar_move(): move\n";
    bar_m(std::move(a));
    bar_m(std::move(r));

    cout << "\nbar_move(): forward\n";
    bar_m(std::forward<decltype(r)>(a));
    bar_m(std::forward<decltype(r)>(r));

    return 0;
}