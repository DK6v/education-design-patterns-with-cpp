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

struct Base {
    virtual ~Base() {}
    virtual std::string whoami() = 0;
};

struct BaseSpecs {
    virtual ~BaseSpecs() = default;
    virtual Type getType() const = 0;
};

class ObjectFactory {
public:
    static ObjectFactory & getInstance() {
        static ObjectFactory instance;
        return instance;
    }

    typedef std::unique_ptr<Base> (*Builder)(const BaseSpecs& specs);

    static void registerBuilder(Type type, Builder builder) {
        register_.insert(std::make_pair(type, builder));
    }

    std::unique_ptr<Base> makeObject(const BaseSpecs & specs) {
        Builder builder = register_[specs.getType()];
        return builder(specs);
    }

private:
    ObjectFactory() = default;
    ObjectFactory(const ObjectFactory&) = delete;
    ObjectFactory& operator=(const ObjectFactory&) = delete;

    static inline std::map<Type, Builder> register_ = {};
};

class A_specs : public BaseSpecs {
public:
    virtual Type getType() const override { return Type::A; }
};

class B_specs : public BaseSpecs {
public:
    virtual Type getType() const override { return Type::B; }
};

class C_specs : public BaseSpecs {
public:
    virtual Type getType() const override { return Type::C; }
};

class A : public Base, printable<A> {
public:
    A(const A_specs&) { std::cout << "-- A::constructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual ~A() { std::cout << "-- A::destructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual std::string whoami() { return "A"; }

    static void registerBuilder() {
        ObjectFactory::getInstance().registerBuilder(Type::A, &A::create);
    }

    static std::unique_ptr<Base> create(const BaseSpecs& specs) {
        return std::make_unique<A>(dynamic_cast<const A_specs &>(specs));

    }
};

class B : public Base, printable<B> {
public:
    B(const B_specs&) { std::cout << "-- B::constructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual ~B() { std::cout << "-- B::destructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual std::string whoami() { return "B"; }

    static void registerBuilder() {
        ObjectFactory::getInstance().registerBuilder(Type::B, &B::create);
    }

    static std::unique_ptr<Base> create(const BaseSpecs& specs) {
        return std::make_unique<B>(dynamic_cast<const B_specs &>(specs));

    }
};

class C : public Base, printable<C> {
public:
    C(const C_specs&) { std::cout << "-- C::constructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual ~C() { std::cout << "-- C::destructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual std::string whoami() { return "C"; }

    static std::unique_ptr<Base> create(const BaseSpecs& specs) {
        return std::make_unique<C>(dynamic_cast<const C_specs &>(specs));

    }

    static void registerBuilder() {
        ObjectFactory::getInstance().registerBuilder(Type::C, &C::create);
    }
};

int main(int argc, char **argv) {

    A::registerBuilder();
    B::registerBuilder();
    C::registerBuilder();

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(ObjectFactory, Test01) {
    auto p = ObjectFactory::getInstance().makeObject(A_specs());
    std::cout << "-- whoami: " << p->whoami() << std::endl;
}

TEST(ObjectFactory, Test02) {
    auto p = ObjectFactory::getInstance().makeObject(B_specs());
    std::cout << "-- whoami: " << p->whoami() << std::endl;
}

TEST(ObjectFactory, Test03) {
    auto p = ObjectFactory::getInstance().makeObject(C_specs());
    std::cout << "-- whoami: " << p->whoami() << std::endl;
}
