#include <assert.h>

#include <iostream>

using namespace std;

template <class T>
class Base {
public:
    Base() : a_{0, 0} {}
    Base(const T& a, const T& b) : a_{a, b} {}

    virtual void name() {
        cout << "Base: " << typeid(decltype(this)).name() << " "
             << hex << a_[0] << " " << a_[1] << dec << endl;
    }

    T sum() {
        return a_[0] + a_[1];
    }

    T subtr() {
        return reinterpret_cast<T>(a_[1] - a_[0]);
    }

public:
    T a_[2];
};

void printHex(void* data, int size) {
    cout << hex;
    for (int ix = 0; ix < size; ++ix) {
        cout << hex << (0xff & static_cast<unsigned char*>(data)[ix]) << " ";
        if ((ix != 0) && ((ix + 1) != size) && ((ix + 1) % 8 == 0)) {
            cout << endl;

        }
    }
    cout << dec;
    cout << endl;
}

int main(int avrc, char** argv) {

    cout << "int sz: " << sizeof(int) << endl;
    cout << "int* sz: " << sizeof(int*) << endl;
    cout << "Base<int> sz: " << sizeof(Base<int>) << endl;

    printHex(new Base<int>(), sizeof(Base<int>));

    Base<int> b1(1, 1);
    b1.name();
    cout << "Sum: " << b1.sum() << endl;

    char str[] = "Hello!";
    Base<char*> b2(str, str + 1);
    b2.name();
    cout << "Sum: " << hex << reinterpret_cast<long>(b2.subtr()) << dec << endl;

    return 0;
}