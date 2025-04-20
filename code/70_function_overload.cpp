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

void foo(long a) {
    std::cout << __FUNCTION__ << " "
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name()
              << std::endl;
}

void foo(double a) {
    std::cout << __FUNCTION__ << " "
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name()
              << std::endl;
}

void foo(int a, int b) {
    std::cout << __FUNCTION__ << " "
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name() << " "
              << boost::typeindex::type_id_with_cvr<decltype(b)>().pretty_name()
              << std::endl;
}

void foo(long a, long b) {
    std::cout << __FUNCTION__ << " "
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name() << " "
              << boost::typeindex::type_id_with_cvr<decltype(b)>().pretty_name()
              << std::endl;
}

void foo(int a, int b, int c) {
    std::cout << __FUNCTION__ << " "
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name() << " "
              << boost::typeindex::type_id_with_cvr<decltype(b)>().pretty_name() << " "
              << boost::typeindex::type_id_with_cvr<decltype(c)>().pretty_name()
              << std::endl;
}

void foo(long a, long b, long c) {
    std::cout << __FUNCTION__ << " "
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name() << " "
              << boost::typeindex::type_id_with_cvr<decltype(b)>().pretty_name() << " "
              << boost::typeindex::type_id_with_cvr<decltype(c)>().pretty_name()
              << std::endl;
}

void foo(...) {
    std::cout << __FUNCTION__ << "(...) "
              << std::endl;
}

TEST(FunctionOverload, Test1) {
    std::cout << "-- int: ";
    foo(static_cast<int>(1));

    std::cout << "-- long: ";
    foo(static_cast<long>(1));

    std::cout << "-- char: ";
    foo(static_cast<char>(1));

    std::cout << "-- float: ";
    foo(static_cast<float>(1));

// compilation error, even if foo(...) exists
// foo(static_cast<unsigned int>(1));

    std::cout << "-- int int: ";
    foo(static_cast<int>(1), static_cast<int>(1));

    std::cout << "-- long long: ";
    foo(static_cast<long>(1), static_cast<long>(1));

    std::cout << "-- int float: ";
    foo(static_cast<int>(1), static_cast<float>(1));

    std::cout << "-- long float: ";
    foo(static_cast<long>(1), static_cast<float>(1));

//  overload is ambiguous:
//  foo(static_cast<int>(1), static_cast<long>(1));
//  foo(static_cast<long>(1), static_cast<int>(1));
//  foo(static_cast<float>(1), static_cast<float>(1));

    std::cout << "-- int int int: ";
    foo(static_cast<int>(1), static_cast<int>(1), static_cast<int>(1));

    std::cout << "-- long long long: ";
    foo(static_cast<long>(1), static_cast<long>(1), static_cast<long>(1));

//  overload is ambiguous:
//  foo(static_cast<int>(1), static_cast<int>(1), static_cast<long>(1));
//  foo(static_cast<int>(1), static_cast<long>(1), static_cast<long>(1));

// If there is an overload for which EACH argument has the best possible conversion,
// then that overload is selected.

}
