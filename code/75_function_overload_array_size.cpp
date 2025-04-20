#include "boost/type_index.hpp"
#include <gtest/gtest.h>

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

template <size_t N>
// Pointer to array
void foo(char(*) [N % 2]= nullptr) {
    std::cout << " * " << __FUNCTION__ << "<ODD>("
              << ")" << std::endl;
}

template <size_t N>
void foo(char(*) [(N + 1) % 2] = nullptr) {
    std::cout << " * " << __FUNCTION__ << "<EVEN>("
              << ")" << std::endl;
}

TEST(FunctionOverloadArraySize, Test1) {

    int a[10] = {1, 2, 3};
    int* b = a;
    int (*p1)[10] = &a;
    int** p2 = &b;

    std::cout << " a[0] = " << a[0] << std::endl;
    std::cout << " b[0] = " << b[0] << std::endl;
    std::cout << "*p1[0] = " << *p1[0] << std::endl;
    std::cout << "*p2[0] = " << *p2[0] << std::endl;

    foo<1>();
    foo<2>();
}
