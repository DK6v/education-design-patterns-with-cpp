#include <assert.h>

#include <iostream>

using namespace std;

class Empty {
public:
    void name() {
        cout << "Empty:" << endl;
    }
};

class EmptyVirtual {
public:
    virtual void name() {
        cout << "EmptyVirtual:" << endl;
    }
};

class BaseA {
public:
    virtual void name() {
        cout << "BaseA:" << a_ << endl;
    }

public:
    int a_ = 0x1A1B1C1D;
};

class BaseB {
public:
    virtual void name() {
        cout << "BaseA:" << b_ << endl;
    }

public:
    short d_ = 0x4A4B;
    int b_ = 0x2A2B2C2D;
    int c_ = 0x3A3B3C3D;
    short e_ = 0x5A5B;
};

class Derived : public Empty,  BaseA, public BaseB {
public:
    void name() override {
        cout << "Derived:" << a_ << b_ << endl;
    }
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

    cout << "Empty sz: " << sizeof(Empty) << endl;
    printHex(new Empty(), sizeof(Empty));

    cout << "EmptyVirtual sz: " << sizeof(EmptyVirtual) << endl;
    printHex(new EmptyVirtual(), sizeof(EmptyVirtual));

    cout << "BaseA sz: " << sizeof(BaseA) << endl;
    printHex(new BaseA(), sizeof(BaseA));

    cout << "BaseB sz: " << sizeof(BaseB) << endl;
    printHex(new BaseB(), sizeof(BaseB));

    cout << "Derived sz: " << sizeof(Derived) << endl;
    printHex(new Derived(), sizeof(Derived));

    return 0;
}