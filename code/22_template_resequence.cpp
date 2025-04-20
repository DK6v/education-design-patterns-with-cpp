#include <assert.h>

#include <iostream>
#include <vector>
#include <list>

using namespace std;

template <template <typename> class OutContainer,
          template <typename> class InContainer,
          typename T>
OutContainer<T> resequence(InContainer<T> in) {
    OutContainer<T> out;
    for (auto & v: in) {
        out.push_back(v);
    }
    return out;
}

int main(int argc, char** argv) {

    (void)argc;
    (void)argv;

    cout << "int sz: " << sizeof(int) << endl;
    cout << "int* sz: " << sizeof(int*) << endl;

    list<int> l = {1, 2, 3};
    auto v = resequence<vector>(l);

    for (auto & item: v) {
        cout << hex << item << dec << " ";
    }
    cout << endl;

    cout << typeid(decltype(l)).name() << endl;
    cout << typeid(decltype(v)).name() << endl;

    return 0;
}