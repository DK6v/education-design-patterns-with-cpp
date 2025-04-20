#include "boost/type_index.hpp"
#include "common.h"

void foo(int&& v) {
    (void)v;
    std::cout << " -> foo(int&& v)" << std::endl;
}

void foo(const int&& v) {
    (void)v;
    std::cout << " -> foo(const int&& v)" << std::endl;
}

void foo(int& v) {
    (void)v;
    std::cout << " -> foo(int& v)" << std::endl;
}

void foo(const int& v) {
    (void)v;
    std::cout << " -> foo(const int& v)" << std::endl;
}

int main(int argc, char** argv) {

    (void)argc;
    (void)argv;

    int a = 1; // lvalue

    std::cout << "== foo() + lvalue";
    foo(a);

    std::cout << "== foo() + rvalue";
    foo(1);

    std::cout << "== foo() + xvalue (move)";
    foo(std::move(a));

    const int&& crv = 1;
    std::cout << "== foo() + const lvalue";
    foo(crv);

    std::cout << "== foo() + const xvalue (move)";
    foo(std::move(crv));

    const int& ca = a;
    std::cout << "== foo() + const rvalue";
    foo(std::move(ca));

    std::cout << "== foo() + const xvalue (move)";
    foo(ca);

    return 0;
}