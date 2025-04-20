#include "boost/type_index.hpp"
#include <gtest/gtest.h>

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

void foo(int a) {
    std::cout << __FUNCTION__ << "("
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name()
              << ")" << std::endl;
}

void foo(long a) {
    std::cout << __FUNCTION__ << "("
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name()
              << ")" << std::endl;
}

template <typename T>
void foo(T a) {
    std::cout << __FUNCTION__ << "<T>("
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name()
              << ")" << std::endl;
}

template <typename T>
void foo(T* a) {
    std::cout << __FUNCTION__ << "<T*>("
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name()
              << ")" << std::endl;
}

TEST(FunctionOverload, Test1) {
    std::cout << "-- int: ";
    foo(static_cast<int>(1));

    std::cout << "-- long: ";
    foo(static_cast<long>(1));

    std::cout << "-- long: ";
    foo<long>(static_cast<long>(1));

    std::cout << "-- double: ";
    foo(static_cast<double>(1));

    int a = 1;
    std::cout << "-- int*: ";
    foo(&a);
}
