#include "boost/type_index.hpp"
#include <gtest/gtest.h>

template <typename T>
struct printable {
    friend std::ostream& operator<<(std::ostream& out, const T& rhs) {
        out << boost::typeindex::type_id_with_cvr<decltype(rhs)>().pretty_name();
        return out;

    }
};

enum class Type : int { A, B, C };

struct BaseSpecs {
    virtual ~BaseSpecs() = default;
    virtual Type getType() const = 0;
};

struct Base {
    virtual ~Base() {}
    virtual std::string whoami() = 0;
};

class A_specs : public BaseSpecs {
    virtual Type getType() const { return Type::A; }
};

class B_specs : public BaseSpecs {
    virtual Type getType() const { return Type::B; }
};

class C_specs : public BaseSpecs {
    virtual Type getType() const { return Type::C; }
};

class A : public Base, printable<A> {
public:
    A(const A_specs&) { std::cout << "-- A::constructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual ~A() { std::cout << "-- A::destructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual std::string whoami() { return "A"; }
};

class B : public Base, printable<B> {
public:
    B(const B_specs&) { std::cout << "-- B::constructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual ~B() { std::cout << "-- B::destructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual std::string whoami() { return "B"; }
};

class C : public Base, printable<C> {
public:
    C(const C_specs&) { std::cout << "-- C::constructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual ~C() { std::cout << "-- C::destructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual std::string whoami() { return "C"; }
};

std::unique_ptr<Base> makeObject(const BaseSpecs & specs) {
    switch(specs.getType()) {
        case Type::A: return std::make_unique<A>(dynamic_cast<const A_specs &>(specs));
        case Type::B: return std::make_unique<B>(dynamic_cast<const B_specs &>(specs));
        case Type::C: return std::make_unique<C>(dynamic_cast<const C_specs &>(specs));
        default: return nullptr;
    }
}

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(ObjectFactory, Test01) {
    A_specs specs;

    auto p = makeObject(specs);
    std::cout << "-- whoami: " << p->whoami() << std::endl;
}

TEST(ObjectFactory, Test02) {
    B_specs specs;
    auto p = makeObject(specs);
    std::cout << "-- whoami: " << p->whoami() << std::endl;
}

TEST(ObjectFactory, Test03) {
    auto p = makeObject(C_specs());
    std::cout << "-- whoami: " << p->whoami() << std::endl;
}
