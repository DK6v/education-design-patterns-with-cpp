#include <gtest/gtest.h>

#include "boost/type_index.hpp"
#include <gtest/gtest.h>

template <class T> concept has_equal_operator = requires (T a) { a == a; };
template <class T> concept has_less_operator = requires (T a) { a < a; };

// Note! Requires cannot be applied here, because the CRTP is used and type is not full;
template <typename T>
struct make_comparable {
    friend bool operator>(const T& lhs, const T& rhs) {
        return !(lhs == rhs) && !(lhs < rhs);
    }
};

template <typename T, char N = 'A'>
class Object : public make_comparable<Object<T, N>> {
public:
    friend make_comparable<Object<T, N>>;

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
    friend bool operator<(const Object<T, N>& lhs, const Object<T, N>& rhs) {
        return lhs.value_ < rhs.value_;
    }

    friend bool operator==(const Object& lhs, const Object& rhs) {
        return lhs.value_ == rhs.value_;

    }

    // Note!!!

    // It is not allowed to define binary operators both as member and free functions simultaneously.

private:
    static inline int total_ = 1;

    int id_;
    T value_;
};

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(FriendFactoryTemplate, Test01) {

    Object a(1), b(2);
    std::cout << "--" << std::endl;

    std::cout << "-- == " << (a == b) << std::endl;
    std::cout << "-- == " << (a < b) << std::endl;
    std::cout << "-- == " << (a > b) << std::endl;

    std::cout << "-- == " << (a == 1) << std::endl;
    std::cout << "-- == " << (a < 1) << std::endl;
    std::cout << "-- == " << (a > 1) << std::endl;

    std::cout << "-- == " << (1 == a) << std::endl;
    std::cout << "-- == " << (1 < a) << std::endl;
    std::cout << "-- == " << (1 > a) << std::endl;

    std::cout << "--" << std::endl;
}
