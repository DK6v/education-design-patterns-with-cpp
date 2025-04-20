#include "boost/type_index.hpp"
#include <gtest/gtest.h>

template <typename T>
class has_unique_id {
public:
    has_unique_id() : id_(++total_) {
        std::cout << "-- has_unique_id::constructor " << std::endl;
    }
    virtual int id() const { return id_; }
private:
    mutable int id_; // cannot be const, otherwise cannot generate default constructors
    static inline int total_ = 0;
};

template <typename T>
struct printable : public has_unique_id<T> {
    friend std::ostream& operator<<(std::ostream& out, const T& rhs) {
        out << boost::typeindex::type_id_with_cvr<decltype(rhs)>().pretty_name()
            << "[" << rhs.id() << "]";
        return out;

    }
};

struct ObjectBase {
    virtual ~ObjectBase() {}
    virtual std::string whoami() = 0;
    virtual std::unique_ptr<ObjectBase> clone() const = 0;
};

enum class Type : int { A, B, C };

struct BuilderBase {
    virtual ~BuilderBase() = default;
    virtual Type getType() const = 0;
    virtual std::unique_ptr<ObjectBase> makeObject() const = 0;
};

template <typename T>
struct cloneable : public ObjectBase {
    virtual std::unique_ptr<ObjectBase> clone() const override {
        return std::unique_ptr<T>(new T(*static_cast<const T*>(this)));
    }
};

class A : public cloneable<A>, public printable<A> {
public:
    A() { std::cout << "-- A::constructor: " << *this << " whoami: " << whoami() << std::endl; }
    explicit A(const A&) { std::cout << "-- A::copy: " << *this << " whoami: " << whoami() << std::endl; }
    virtual ~A() { std::cout << "-- A::destructor: " << *this << " whoami: " << whoami() << std::endl; }

    A& operator=(const A&) {
        std::cout << "-- A::operator=: " << *this << " whoami: " << whoami() << std::endl;
        return *this;
    }

    virtual std::string whoami() { return "A"; }
};

class B : public ObjectBase, public printable<B> {
public:
    B() { std::cout << "-- B::constructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual ~B() { std::cout << "-- B::destructor: " << *this << " whoami: " << whoami() << std::endl; }

    explicit B(const B&) = default;
    B& operator=(const B&) = default;

    virtual std::string whoami() { return "B"; }
    virtual std::unique_ptr<ObjectBase> clone() const { return std::unique_ptr<ObjectBase>(new B(*this)); }
};

class C : public ObjectBase, public printable<C> {
public:
    C() { std::cout << "-- C::constructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual ~C() { std::cout << "-- C::destructor: " << *this << " whoami: " << whoami() << std::endl; }
    virtual std::string whoami() { return "C"; }
    virtual std::unique_ptr<ObjectBase> clone() const { return std::unique_ptr<ObjectBase>(new C(*this)); }
};

class ObjectFactory {
public:
    static ObjectFactory & getInstance() {
        static ObjectFactory instance;
        return instance;
    }

    template <typename ConcreteBuilder>
    void registerBuilder() {
        builders_.insert(
            std::pair(ConcreteBuilder::type,
                      std::make_unique<builder<ConcreteBuilder>>()));
    }

    std::unique_ptr<ObjectBase> makeObject(const BuilderBase & specs) {
        if (builders_[specs.getType()] == nullptr) {
            std::cout << "-- invalid builder type (NULLPTR)" << std::endl;
            return nullptr;
        }
        return builders_[specs.getType()]->makeObject(specs);
    }

private:
    struct builder_base {
        virtual std::unique_ptr<ObjectBase> makeObject(const BuilderBase& specs) const = 0;
    };

    template <typename ConcreteBuilder>
    struct builder : public builder_base {
        builder() : cb_(&ConcreteBuilder::makeObject) {}
        virtual std::unique_ptr<ObjectBase> makeObject(const BuilderBase& specs) const override {
            return (dynamic_cast<const ConcreteBuilder&>(specs).*cb_) ();
        }
        std::unique_ptr<ObjectBase> (ConcreteBuilder::*cb_) () const;
    };

    static inline std::map<Type, std::unique_ptr<builder_base>> builders_ = {};

private:
    // Singleton
    ObjectFactory() = default;
    ObjectFactory(const ObjectFactory&) = delete;
    ObjectFactory& operator=(const ObjectFactory&) = delete;
};

template <typename T, Type ID>
class Builder : public BuilderBase {
public:
    static inline Type type = ID;
    virtual Type getType() const override { return type; }

    std::unique_ptr<ObjectBase> makeObject() const override {
        return std::make_unique<T>();

    }
};

class A_Builder : public Builder<A, Type::A> {};
class B_Builder : public Builder<B, Type::B> {};
class C_Builder : public Builder<C, Type::C> {};

int main(int argc, char **argv) {

    ObjectFactory::getInstance().registerBuilder<A_Builder>();
    ObjectFactory::getInstance().registerBuilder<B_Builder>();
    ObjectFactory::getInstance().registerBuilder<C_Builder>();

    std::cout << "-- Registered" << std::endl;

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(ObjectFactory, Test01) {
    auto p = ObjectFactory::getInstance().makeObject(A_Builder());
    std::cout << "-- whoami: " << p->whoami() << std::endl;
}

TEST(ObjectFactory, Test02) {
    auto p = ObjectFactory::getInstance().makeObject(B_Builder());
    std::cout << "-- whoami: " << p->whoami() << std::endl;
}

TEST(ObjectFactory, Test03) {
    auto p = ObjectFactory::getInstance().makeObject(C_Builder());
    std::cout << "-- whoami: " << p->whoami() << std::endl;
}

TEST(ObjectFactory, Test04) {
    auto p = ObjectFactory::getInstance().makeObject(A_Builder());
    auto copy = p->clone();
    std::cout << "-- whoami: " << copy->whoami() << std::endl;
}
