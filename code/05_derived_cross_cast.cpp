#include <assert.h>

#include <iostream>

using namespace std;

class BaseA {
public:
    virtual void name() {
        cout << "BaseA:" << hex << a_ << dec << endl;
    }

    void type() {
        BaseA::name();
    }

public:
    int a_ = 0x1A1B1C1D;
};

class BaseB {
public:
    virtual void name() {
        cout << "BaseB:" << hex << b_ << dec << endl;
    }

    void type() {
        BaseB::name();
    }

public:
    int b_ = 0x2A2B2C2D;
};

class Derived : public BaseA, public BaseB {
public:
    void name() override {
        cout << "Derived:" << hex << a_ << " " << b_ << dec << endl;
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

    cout << "BaseA sz: " << sizeof(BaseA) << endl;
    printHex(new BaseA(), sizeof(BaseA));

    cout << "BaseB sz: " << sizeof(BaseB) << endl;
    printHex(new BaseB(), sizeof(BaseB));

    cout << "Derived sz: " << sizeof(Derived) << endl;
    printHex(new Derived(), sizeof(Derived));

    BaseA* a = new Derived();
    a->type();

    BaseB* b = dynamic_cast<BaseB*>(a);
    b->type();

    BaseB* c = reinterpret_cast<BaseB*>(a);
    c->type();

    // BaseB* d = static_cast<BaseB*>(a); <= ERROR: invalid static cast

    return 0;
}