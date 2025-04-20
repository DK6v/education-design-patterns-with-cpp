#include <gtest/gtest.h>

#include "boost/type_index.hpp"
#include <gtest/gtest.h>

namespace app {

template <typename T, char N = 'A'>
class Object {
public:
    Object(const T& value) : id_(total_++), value_(value) {
        std::cout << "-- " << *this << ": constructor" << std::endl;
    }
    ~Object() {
        std::cout << "-- " << *this << ": destructor" << std::endl;
    }
    Object(const Object& other) : id_(total_++) {
        std::cout << "-- " << *this << ": copy constructor [" << other.id << "]" << std::endl;
    }
    Object(Object&& other) : id_(total_++) {
        std::cout <<"-- " << *this << ": move constructor [" << other.id << "]"<< std::endl;
    }

    const T& operator()() { return value_; }

    friend std::ostream& operator<<(std::ostream& out, const Object& rhs) {
        out << "Object<" << N << ">[" << rhs.id_ << "] { value_: " << rhs.value_ << " }";
        return out;

    }

    // Friend factory,
    // Note! Functions cannot be declared outside of the class.
    // Note! Binary function cannot be static.
    friend Object operator+(const Object& lhs, const Object& rhs) {
        return lhs.value_ + rhs.value_;

    }
    friend Object<T, N> operator*(const Object<T, N>& lhs, const Object<T, N>& rhs) {
        return lhs.value_ * rhs.value_;
    }

private:
    static inline int total_ = 1;

    int id_;
    T value_;
};

} // namespace app

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(FriendFactoryTemplate, Test01) {

    app::Object a(1);
    std::cout << "-- " << a << ": Hello!!!" << std::endl;
    std::cout << "--" << std::endl;

    // Note! The operator '+' is available, because the compiler uses
    //       functions from the current namespace and the namespace of the parameters.

    app::Object b = a + a;
    std::cout << "-- " << b << std::endl;
    std::cout << "--" << std::endl;

    app:: Object c = a + 1;
    std::cout << "-- " << c << std::endl;
    std::cout << "--" << std::endl;

    app::Object d = 1 + a;
    std::cout << "-- " << d << std::endl;
    std::cout << "--" << std::endl;
}

TEST(FriendFactoryTemplate, Test02) {

    app::Object a(1);
    std::cout << "-- " << a << ": Hello!!!" << std::endl;
    std::cout << "--" << std::endl;

    app::Object b(2);
    std::cout << "-- " << b << std::endl;
    std::cout << "--" << std::endl;

    app::Object c = a * b;
    std::cout << "-- " << c << std::endl;
    std::cout << "--" << std::endl;

    app::Object d = 2 * a;
    std::cout << "-- " << d << std::endl;
    std::cout << "--" << std::endl;

    app::Object e = a * 2;
    std::cout << "-- " << d << std::endl;
    std::cout << "--" << std::endl;
}

TEST(FriendFactoryTemplate, Test03) {

    app::Object a(1), b(1);
    std::cout << "--" << std::endl;

    app::Object c = operator+(a, b);
    std::cout << "-- " << c << std::endl;
    std::cout << "--" << std::endl;
}
