#include "boost/type_index.hpp"
#include <gtest/gtest.h>

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

void foo() {
    std::cout << " * " << __FUNCTION__ << "("
              << ")" << std::endl;
}

template <typename H, typename... T>
void foo(H a, T... args) {
    std::cout << " * " << __FUNCTION__ << "("
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name()
              << ")" << std::endl;
    foo(args...);
}

void bar(int a) {
    std::cout << " * " << __FUNCTION__ << "("
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name()
              << ")" << std::endl;
}

void bar(...) {
    std::cout << " * " << __FUNCTION__ << "(...)" << std::endl;
}

TEST(FunctionOverloadVarg, Test1) {
    std::cout << "-- int: " << std::endl;
    foo(static_cast<int>(1));

    std::cout << "-- long: " << std::endl;
    foo(static_cast<long>(1));

    std::cout << "-- int long int long: " << std::endl;
    foo(static_cast<int>(1), static_cast<long>(1), static_cast<int>(1), static_cast<long>(1));

    std::cout << "-- int: " << std::endl;
    bar(static_cast<int>(1));

    std::cout << "-- double: " << std::endl;
    // << will cast to int, never call bar(...) if possible
    bar(static_cast<double>(1));

    std::cout << "-- std::string: " << std::endl;
    bar(std::string("Hello"));
}
