#include <assert.h>

#include <iostream>
#include <vector>
#include <list>

using namespace std;

template <typename A, typename B>
auto average(A* a, B* b) {
    cout << "average2_ptr(" << typeid(A).name() << ", " << typeid(B).name() << "): ";
    return (*a + *b) / 2;
}

template <typename A, typename B>
auto average(A a, B b) {
    cout << "average2(" << typeid(A).name() << ", " << typeid(B).name() << "): ";
    return (a + b) / 2;
}

int main(int argc, char** argv) {

    (void)argc;
    (void)argv;

    cout << "int sz: " << sizeof(int) << endl;
    cout << "int* sz: " << sizeof(int*) << endl;

    cout << hex << average(1, 2) << dec << endl;
    cout << hex << average<double>(1, 2) << dec << endl;

    int a = 1;
    int b = 2;
    cout << hex << average(a, b) << dec << endl;
    cout << hex << average(&a, &b) << dec << endl;

    return 0;
}