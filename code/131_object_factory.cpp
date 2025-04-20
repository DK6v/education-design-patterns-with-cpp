#include "boost/type_index.hpp"
#include <gtest/gtest.h>

template <typename T>
struct printable {
    friend std::ostream& operator<<(std::ostream& out, const T& rhs) {
        out << boost::typeindex::type_id_with_cvr<decltype(rhs)>().pretty_name();
        return out;

    }
};

struct Base {
    virtual ~Base() {}
    virtual std::string whoami() = 0;
};

class A : public Base, printable<A> {
public:
    A() { std::cout << "-- A::constructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual ~A() { std::cout << "-- A::destructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual std::string whoami() { return "A"; }
};

class B : public Base, printable<B> {
public:
    B() { std::cout << "-- B::constructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual ~B() { std::cout << "-- B::destructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual std::string whoami() { return "B"; }
};

class C : public Base, printable<C> {
public:
    C() { std::cout << "-- C::constructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual ~C() { std::cout << "-- C::destructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual std::string whoami() { return "C"; }
};

enum class Type : int { A, B, C };

std::unique_ptr<Base> makeObject(Type type) {
    switch(type) {
        case Type::A: return std::make_unique<A>();
        case Type::B: return std::make_unique<B>();
        case Type::C: return std::make_unique<C>();
        default: return nullptr;
    }
}

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(ObjectConstructionOrder, Test01) {
    auto p = makeObject(Type::A);
    std::cout << "-- whoami: " << p->whoami() << std::endl;
}

TEST(ObjectConstructionOrder, Test02) {
    auto p = makeObject(Type::B);
    std::cout << "-- whoami: " << p->whoami() << std::endl;
}

TEST(ObjectConstructionOrder, Test03) {
    auto p = makeObject(Type::C);
    std::cout << "-- whoami: " << p->whoami() << std::endl;
}
