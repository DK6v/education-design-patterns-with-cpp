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
    cout << "Base sz: " << sizeof(Base) << endl;
    cout << "Derived sz: " << sizeof(Derived) << endl;

    Base b1 = Base();
    b1.name();

    Derived b2 = Derived();
    b2.name();

    Base& b3 = b1;
    Base& b4 = b2;

    Derived& d1 = static_cast<Derived&>(b2);
    Derived& d2 = dynamic_cast<Derived&>(b2);

    Derived& d3 = static_cast<Derived&>(b4);
    Derived& d4 = dynamic_cast<Derived&>(b4);

    Derived& d5 = static_cast<Derived&>(b3); // <= undefined behavior
    try {
        Derived& d6 = dynamic_cast<Derived&>(b3); // <= bad_cast exception

        // exit code 134, SIGABRT(6) = 134 - 128
    }
    catch (const std::exception& err) {
        cout << err.what() << endl;
    }

    return 0;
}