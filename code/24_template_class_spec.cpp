#include <assert.h>

#include <iostream>
#include <vector>
#include <list>

using namespace std;

template <typename A, typename B>
class Average {
public:
    Average(A a, B b) : a_(a), b_(b) {
        cout << "Average1(" << typeid(A).name() << ", " << typeid(B).name() << "): ";
    }
    operator double() {
        return (a_ + b_) / 2;
    }

private:
    A a_;
    B b_;
};

template <>
Average<float, float>::operator double() {
    cout << "Average(float): ";
    return (a_ + b_) / 2;
}

template <typename A, typename B>
class Average<A*, B*> {
public:
    Average(A* a, B* b) : a_(*a), b_(*b) {
        cout << "Average2(" << typeid(A).name() << ", " << typeid(B).name() << "): ";
    }
    operator double() {
        return (a_ + b_) / 2;
    }

private:
    A a_;
    B b_;
};

template <typename A, typename B>
Average<A, B> make_average(A a, B b) {
    return {a, b};
}

int main(int argc, char** argv) {

    (void)argc;
    (void)argv;

    cout << "int sz: " << sizeof(int) << endl;
    cout << "int* sz: " << sizeof(int*) << endl;

    cout << "-- Average r1(1, 2)"  << endl;
    Average r1(1, 2);
    cout << hex << static_cast<double>(r1) << dec << endl;
    // => 1

    cout << "-- Average<double, double> r2(1, 2)"  << endl;
    Average<double, double> r2(1, 2);
    cout << hex << static_cast<double>(r2) << dec << endl;

    cout << "-- Average r2_1(1.0, 2.0)"  << endl;
    Average r2_1(1.0, 2.0);
    cout << hex << static_cast<double>(r2) << dec << endl;
    // => 1.5

    auto r3 = make_average(1, 2);
    cout << hex << static_cast<double>(r3) << dec << endl;
    // => 1

    auto r4 = make_average<double>(1, 2);
    cout << hex << static_cast<double>(r4) << dec << endl;
    cout << hex << r4 << dec << endl;
    // => 1.5

    int v1 = 1;
    int v2 = 2;

    auto r5 = make_average(v1, v2);
    cout << hex << r5 << dec << endl;

    auto r6 = make_average<double>(v1, v2);
    cout << hex << r6 << dec << endl;

    auto r7 = make_average(&v1, &v2);
    cout << hex << r7 << dec << endl;

    auto r8 = Average<int*, int*>(&v1, &v2);
    cout << hex << r8 << dec << endl;

    auto r9 = make_average<int*>(&v1, &v2);
    cout << hex << r9 << dec << endl;

    auto r10 = Average<float, float>(1.0, 2.0);
    cout << hex << r10 << dec << endl;

    return 0;
}