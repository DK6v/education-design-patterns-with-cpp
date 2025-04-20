#include <assert.h>

#include <iostream>

using namespace std;

class Base {
public:
    virtual void name() {
        cout << "BaseA:" << hex << Base::a_<int> << dec << endl;
        cout << "BaseA:" << hex << Base::a_<long> << dec << endl;
    }

public:
    template <typename T>
    static const T a_ = T(0x1A1B1C1D);
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

    cout << "BaseA sz: " << sizeof(Base) << endl;
    printHex(new Base(), sizeof(Base));

    Base b;
    b.name();

    return 0;
}