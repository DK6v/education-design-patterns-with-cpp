#include <iostream>

using namespace std;

class Base {
public:
    virtual void name() {
        a_++;
        b_++;
        c_++;
        cout << "Base:" << a_ << b_ << c_ << endl;
    }

public:
    int a_ = 1;
protected:
    int b_ = 1;
private:
    int c_ = 1;
};

class DerivedPublic : public Base {
public:
    virtual void name() {
        a_++;
        b_++;

        // error: ‘int Base::c_’ is private within this context
        // c_++;

        cout << "DerivedPublic:" << a_ << b_ << endl;
    }

    virtual void name2() {
        cout << "DerivedPublic:" << a_ << b_ << endl;
    }
};

class DerivedPrivate : private Base {
public:
    virtual void name() {
        a_++;
        b_++;

        // error: ‘int Base::c_’ is private within this context
        // c_++;

        cout << "DerivedPrivate:" << a_ << b_ << endl;
    }
};

int main(int avrc, char** argv) {

    Base b;
    b.name();

    DerivedPublic d1;
    d1.name();

    DerivedPrivate d2;
    d2.name();

    static_cast<Base>(d1).name();

    // error: ‘Base’ is an inaccessible base of ‘DerivedPrivate’
    // static_cast<Base>(d2).name();

    Base * pb = new Base();
    pb->name();
    static_cast<DerivedPublic*>(pb)->name();

    return 0;
}