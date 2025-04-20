#include "boost/type_index.hpp"
#include <gtest/gtest.h>

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

void foo(int a) {
    std::cout << __FUNCTION__ << " "
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name()
              << std::endl;
}

void foo(long a, long b) {
    std::cout << __FUNCTION__ << " "
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name() << " "
              << boost::typeindex::type_id_with_cvr<decltype(b)>().pretty_name()
              << std::endl;
}

void foo(double a, double b = 0.0) {
    std::cout << __FUNCTION__ << " "
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name() << " "
              << boost::typeindex::type_id_with_cvr<decltype(b)>().pretty_name() << "=0.0"
              << std::endl;
}

void foo(int a, long b) {
    std::cout << __FUNCTION__ << " "
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name() << " "
              << boost::typeindex::type_id_with_cvr<decltype(b)>().pretty_name()
              << std::endl;
}

void foo(long a, int b) {
    std::cout << __FUNCTION__ << " "
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name() << " "
              << boost::typeindex::type_id_with_cvr<decltype(b)>().pretty_name()
              << std::endl;
}

TEST(FunctionOverload, Test1) {
    std::cout << "-- int: ";
    foo(static_cast<int>(1));

    std::cout << "-- long long: ";
    foo(static_cast<long>(1), static_cast<long>(1));

    std::cout << "-- int long: ";
    foo(static_cast<int>(1), static_cast<long>(1));

    std::cout << "-- long int: ";
    foo(static_cast<long>(1), static_cast<int>(1));

    std::cout << "-- double: ";
    foo(static_cast<double>(1));

    std::cout << "-- float: ";
    foo(static_cast<float>(1));
}

void bar(int a, int b, int c) {
    std::cout << __FUNCTION__ << " "
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name() << " "
              << boost::typeindex::type_id_with_cvr<decltype(b)>().pretty_name() << " "
              << boost::typeindex::type_id_with_cvr<decltype(c)>().pretty_name()
              << std::endl;
}

void bar(long a, long b, long c) {
    std::cout << __FUNCTION__ << " "
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name() << " "
              << boost::typeindex::type_id_with_cvr<decltype(b)>().pretty_name() << " "
              << boost::typeindex::type_id_with_cvr<decltype(c)>().pretty_name()
              << std::endl;
}

TEST(FunctionOverload, Test2) {
    std::cout << "-- int int int: ";
    bar(static_cast<int>(1), static_cast<int>(1), static_cast<int>(1));

    std::cout << "-- long long long: ";
    bar(static_cast<long>(1), static_cast<long>(1), static_cast<long>(1));

    std::cout << "-- int int double: ";
    bar(static_cast<int>(1), static_cast<int>(1), static_cast<double>(1));

    std::cout << "-- int double double: ";
    bar(static_cast<int>(1), static_cast<double>(1), static_cast<double>(1));

//  Compilation error !!!
//  bar(static_cast<int>(1), static_cast<int>(1), static_cast<long>(1));
//  bar(static_cast<long>(1), static_cast<long>(1), static_cast<int>(1));
}