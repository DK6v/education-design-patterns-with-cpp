#include "boost/type_index.hpp"
#include <gtest/gtest.h>

#include <vector>
#include <list>

template <typename T, typename N>
auto if_plus_operator(const T& t, N& n) -> decltype( t + n, std::true_type());
template <typename T, typename N>
std::false_type if_plus_operator(...);

template <typename T, typename N>
auto if_plus_equal_operator(const T& t, N& n) -> decltype( T() += n, std::true_type());
template <typename T, typename N>
std::false_type if_plus_equal_operator(...);

template <typename T, bool if_plus_operator, bool if_plus_equal_operator>
struct increase_helper;

template <typename T, bool if_plus_equal_operator>
struct increase_helper<T, true, if_plus_equal_operator> {
    static inline void f(const T& x, size_t n) {
        (void)(x + n);
        std::cout << "if_plus_operator: true" << std::endl;
    }
};

template <typename T>
struct increase_helper<T, false, true> {
    static inline void f(const T& x, size_t n) {
        (void)(T(x) += n);
        std::cout << "if_plus_equal_operator: true" << std::endl;
    }
};

template<typename T>
auto increase(const T& x, size_t n) {
    return increase_helper<T,
        decltype(if_plus_operator<T, size_t>(x, n))::value,
        decltype(if_plus_equal_operator<T, size_t>(x, n))::value>
    ::f(x, n);
}

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

struct A {
    A& operator+=(const size_t) { return *this; }
};

struct B {
    B operator+(const size_t) const { return B(); }
};

TEST(FunctionOverloadDecltype, Test1) {
    increase(A(), 1);
    increase(B(), 1);
}
