#include <assert.h>

#include <iostream>

using namespace std;

class Base {
public:
    virtual void name() {
        cout << "Base:" << a_ << endl;
    }

public:
    int a_ = 1;
};

class Derived : public Base {
public:
    void name() override {
        cout << "Derived:" << a_ << endl;
    }
};

int main(int avrc, char** argv) {

    cout << "int sz: " << sizeof(int) << endl;
    cout << "int* sz: " << sizeof(int*) << endl;
    cout << "Base sz: " << sizeof(Base) << endl;
    cout << "Derived sz: " << sizeof(Derived) << endl;

    Base* b1 = new Base();
    b1->name();

    Base* b2 = new Derived();
    b2->name();

    Derived* d1 = static_cast<Derived*>(b2);
    assert(d1 != nullptr);

    Derived* d2 = dynamic_cast<Derived*>(b2);
    assert(d2 != nullptr);

    Derived* d3 = static_cast<Derived*>(b1);
    assert(d3 != nullptr);

    Derived* d4 = dynamic_cast<Derived*>(b1); // == nullptr
    assert(d4 != nullptr);

    return 0;
}