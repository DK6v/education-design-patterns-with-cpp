#include "boost/type_index.hpp"
#include <gtest/gtest.h>

#include <string>

template <typename T>
class has_unique_id {
public:
    has_unique_id() : id_(++total_) {}
    int uid() const { return id_; }
private:
    mutable int id_; // cannot be const, otherwise cannot generate default constructors
    static inline int total_ = 0;
};

template <typename T>
struct printable : private has_unique_id<T> {
    friend std::ostream& operator<<(std::ostream& out, const T& rhs) {
        return out << boost::typeindex::type_id<decltype(rhs)>().pretty_name()
                   << "[" << rhs.uid() << "]";
    }
};

class A;
class B;
class Visitor {
public:
    virtual void visit(A&) = 0;
    virtual void visit(B&) = 0;
};

class Visitor;
class Base {
public:
    virtual ~Base() = default;

    virtual void whoami() = 0;
    virtual void accept(Visitor& v) = 0;
};

class A : public Base,
          public printable<A> {
public:
    A() { std::cout << *this << ": constructor" << std::endl; };
    virtual ~A() { std::cout << *this << ": destructor" << std::endl; };

    virtual void whoami() override { std::cout << *this << std::endl; }
    virtual void accept(Visitor& v) override { v.visit(*this); }
};

class B : public Base,
          public printable<B> {
public:
    B() { std::cout << *this << ": constructor" << std::endl; };
    virtual ~B() { std::cout << *this << ": destructor" << std::endl; };

    virtual void whoami() override { std::cout << *this << std::endl; }
    virtual void accept(Visitor& v) override { v.visit(*this); }
};

class TestVisitor : public Visitor {
public:
    virtual void visit(A& a) override { std::cout << a << " visited" << std::endl; }
    virtual void visit(B& b) override { std::cout << b << " visited" << std::endl; }
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(Visitor, Test01) {
    {
        TestVisitor visitor;

        std::unique_ptr<Base> pa(new A());
        pa->accept(visitor);

        std::unique_ptr<Base> pb(new B());
        pb->accept(visitor);
    }
    std::cout << "-- DONE" << std::endl;
}
