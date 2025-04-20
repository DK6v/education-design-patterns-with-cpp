#include "boost/type_index.hpp"
#include <gtest/gtest.h>

class has_unique_id {
public:
    has_unique_id() : id_(++total_) {}
    virtual int id() const { return id_; }
private:
    mutable int id_; // cannot be const, otherwise cannot generate default constructors
    static inline int total_ = 0;
};

template <typename T>
struct printable {
    friend std::ostream& operator<<(std::ostream& out, const T& rhs) {
        out << boost::typeindex::type_id_with_cvr<decltype(rhs)>().pretty_name()
            << "[" << rhs.id() << "]";
        return out;

    }
};

class Base : public has_unique_id, public printable<Base> {
public:
    Base(int value = 0) : value_(value) { std::cout << "-- " << *this << ": constructor" << std::endl; };
    virtual ~Base() { std::cout << "-- " << *this << ": destructor" << std::endl; };

    Base(Base&& other) noexcept : value_(other.value_) {

        std::cout << "-- " << *this << ": constructor" << std::endl;
    }

    virtual void whoami() { std::cout << *this << " value: " << value_ << std::endl; }

public:
    int value_;
};

class Derived : public Base, public printable<Derived> {
    public:
        Derived(int value = 0) : Base(value) {
            std::cout << "-- " << *this << ": constructor" << std::endl;
        };
        Derived(Derived&& other) noexcept : Base(std::move(other)) {

            std::cout << "-- " << *this << ": constructor" << std::endl;
        }

        virtual ~Derived() {
            std::cout << "-- " << *this << ": destructor" << std::endl;
        };

        virtual void whoami() { std::cout << *this << " value: " << value_ << std::endl; }

    };

template<typename U>
class Decorator_A : public U {
public:
    template <typename P>
    Decorator_A(P&& ptr) : U(std::move(*move_cast<U>(ptr))) {}

    void whoami() override {
        std::cout << "Decorated A => ";
        U::whoami();
    }
};

template<typename U>
class Decorator_B : public U {
public:
    template <typename P>
    Decorator_B(P&& ptr) : U(std::move(*move_cast<U>(ptr))) {}

    void whoami() override {
        std::cout << "Decorated B => ";
        U::whoami();
    }
};

template <typename To, typename From>
std::unique_ptr<To> move_cast(std::unique_ptr<From>& ptr) {
    auto pval = dynamic_cast<To*>(ptr.release());
    assert(pval);
    return std::unique_ptr<To>(pval);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(Decorator, Test01) {
    {
        std::unique_ptr<Base> base_p(new Base(1));
        base_p->whoami();
    }
    std::cout << "-- end of context" << std::endl;
}

TEST(Decorator, Test02) {
    {
        std::unique_ptr<Derived> derived_p(new Derived(2));
        derived_p->whoami();

        std::cout << "-- move cast Derived -> Base" << std::endl;
        std::unique_ptr<Base> base_p(std::move(derived_p));
        base_p->whoami();
    }
    std::cout << "-- end of context" << std::endl;
}

TEST(Decorator, Test03) {
    {
        std::unique_ptr<Base> base_p(new Derived(3));
        base_p->whoami();

        std::cout << "-- move cast Base -> Derived" << std::endl;
        std::unique_ptr<Derived> derived_p(move_cast<Derived>(base_p));
        derived_p->whoami();
    }
    std::cout << "-- end of context" << std::endl;
}

TEST(Decorator, Test04) {
    {
        std::unique_ptr<Base> base_p(new Base(4));
        base_p->whoami();

        std::unique_ptr<Base> da_p(new Decorator_A<Base>(base_p));
        da_p->whoami();

        std::unique_ptr<Base> db_p(new Decorator_B<Decorator_A<Base>>(da_p));
        db_p->whoami();
    }
    std::cout << "-- end of context" << std::endl;
}
