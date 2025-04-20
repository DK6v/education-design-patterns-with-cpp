#include "boost/type_index.hpp"
#include <gtest/gtest.h>

template <typename T>
class has_unique_id {
public:
    has_unique_id() : id_(++total_) {}
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

class Base : public printable<Base> {
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

template<typename U>
class Decorator_A : public U {
public:
    Decorator_A(U&& base) : U(std::move(base)) {}

    void whoami() override {
        std::cout << "Decorated A => ";
        U::whoami();
    }
};

template<typename U>
class Decorator_B : public U {
public:
    Decorator_B(U&& base) : U(std::move(base)) {}

    void whoami() override {
        std::cout << "Decorated B => ";
        U::whoami();
    }
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(Decorator, Test01) {
    {
        Base base(1);
        base.whoami();

        Decorator_A da(std::move(base));
        da.whoami();

        Decorator_B db(std::move(da));
        db.whoami();
    }
    std::cout << "-- end of context" << std::endl;
}
