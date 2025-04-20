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
    Base() : value(0) { std::cout << "-- " << *this << ": constructor" << std::endl; };
    virtual ~Base() { std::cout << "-- " << *this << ": destructor" << std::endl; };
public:
    int value;
};

template <typename T, typename V>
struct Incremental {
    V& operator++() { // prefix
        V& v = static_cast<V&>(*this);

        ++(v.get());
        return v;
    }

    V operator++(int) { // postfix
        V& v = static_cast<V&>(*this);

        return V((v.get())++);
    }
};

template <typename T, template <typename, typename> class ... Policies>
class Variable : public Policies<T, Variable<T, Policies ... >> ... {
public:
    explicit Variable() : value_(T()) {}
    explicit Variable(const T& value) : value_(value) {}
    explicit Variable(const Variable& rhs) : value_(rhs.value_) {}

    Variable& operator=(const T& value) { value_ = value; return *this; }
    Variable& operator=(const Variable& rhs) { value_ = rhs.value_; return *this; }

    operator T() const { return value_; }
    operator T&() { return value_; }

    T get() const { return value_; }
    T& get() { return value_; }

private:
    T value_;
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(StrategyTemplate, Test01) {
    {
        Variable<int> a(1);

        a = 2;
        EXPECT_EQ(2, a.get());
    }
    std::cout << "--" << std::endl;
}

TEST(StrategyTemplate, Test02) {
    {
        Variable<int, Incremental> a(1);

        a++;
        EXPECT_EQ(2, a.get());

        ++a;
        EXPECT_EQ(3, a.get());

        auto b = a++;
        EXPECT_EQ(4, a.get());
        EXPECT_EQ(3, b.get());
    }
    std::cout << "--" << std::endl;
}
