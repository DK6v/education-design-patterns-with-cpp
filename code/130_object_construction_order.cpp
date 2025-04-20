#include "boost/type_index.hpp"
#include <gtest/gtest.h>

template <typename T>
struct printable {
    friend std::ostream& operator<<(std::ostream& out, const T& rhs) {
        out << boost::typeindex::type_id_with_cvr<decltype(rhs)>().pretty_name();
        return out;

    }
};

class A : printable<A> {
public:
    A() { std::cout << "-- A::constructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual ~A() { std::cout << "-- A::destructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual std::string whoami() { return "A"; }
};

class B : public A, printable<B> {
public:
    B() { std::cout << "-- B::constructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual ~B() { std::cout << "-- B::destructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual std::string whoami() { return "B"; }
};

class C : public B, printable<C> {
public:
    C() { std::cout << "-- C::constructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual ~C() { std::cout << "-- C::destructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual std::string whoami() { return "C"; }
};

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(ObjectConstructionOrder, Test01) {
    A a;
}

TEST(ObjectConstructionOrder, Test02) {
    B b;
}

TEST(ObjectConstructionOrder, Test03) {
    C c;
}
