#include <assert.h>

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>

using namespace std;

class Base {
public:
    void operator()() {
        cout << "Base: functor" << endl;
    }
};

class Compare {
public:
    bool operator()(int a, int b) const { return a < b; }
};

class CompareAuto {
public:
    template <typename T>
    bool operator()(T a, T b) const { return a < b; }
};

template <typename T>
bool compare(T a, T b) {
    return a < b;
}

template <typename C>
void print(C& container) {
    cout << "Containter: ";
    for (auto & item: container) {
        cout << item << " ";
    }
    cout << endl;
}

int main(int argc, char** argv) {

    (void)argc;
    (void)argv;

    cout << "int sz: " << sizeof(int) << endl;
    cout << "int* sz: " << sizeof(int*) << endl;

    Base b;
    b();

    vector<int> l1 = {3, 2, 1};
    print(l1);
    sort(l1.begin(), l1.end(), compare<int>);
    print(l1);

    vector<int> l2 = {6, 5, 4};
    print(l2);
    sort(l2.begin(), l2.end(), Compare());
    print(l2);

    struct CompareLocal {
        bool operator()(int a, int b) { return a < b; }
    };

    vector<int> l3 = {9, 8, 7};
    print(l3);
    sort(l3.begin(), l3.end(), CompareLocal());
    print(l3);

    auto compareL4 = [](int a, int b) { return a < b; };
    vector<int> l4 = {12, 11, 10};
    print(l4);
    sort(l4.begin(), l4.end(), compareL4);
    print(l4);

    vector<int> l5 = {15, 14, 13};
    print(l5);
    sort(l5.begin(), l5.end(), CompareAuto());
    print(l5);

    auto compareL6 = [](auto a, auto b) { return a < b; };
    vector<int> l6 = {18, 17, 16};
    print(l6);
    sort(l6.begin(), l6.end(), compareL6);
    print(l6);

    return 0;
}