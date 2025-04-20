#include "boost/type_index.hpp"
#include <gtest/gtest.h>

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

template <typename T>
void foo(T* a) {
    std::cout << " * " << __FUNCTION__ << "<T*>("
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name()
              << ")" << std::endl;
}

template <typename T>
// Pointer to class member
void foo(T a, int T::* ptr) {
    std::cout << " * " << __FUNCTION__ << "<int T::*>("
              << boost::typeindex::type_id_with_cvr<decltype(a)>().pretty_name() << " = "
              << a.*ptr
              << ")" << std::endl;
}

template <typename T>
void foo(decltype (T::i) i) {
    std::cout << " * " << __FUNCTION__ << "<typename T::i>("
              << boost::typeindex::type_id_with_cvr<decltype(i)>().pretty_name()
              << ")" << std::endl;
}

struct A {
    int i;
    int j;
    long t;

    typedef decltype(i) type;
};

TEST(FunctionOverloadSfinae, Test1) {

    A a = { .i = 1, .j = 2, .t = 3};

    std::cout << "-- &a: ";
    foo(&a);

    std::cout << "-- &a.i: ";
    foo(&a.i);

    std::cout << "-- struct A, &a.i: ";
    foo(a, &A::i);

    std::cout << "-- struct A, &a.j: ";
    foo(a, &A::j);

    std::cout << "-- struct A, a.i";
    foo<A>(a.i);

    std::cout << "-- struct A, a.j";
    foo<A>(a.j);

    // std::cout << "-- struct *A::t: ";
    // foo(&A::t); // overload not found for type (long A::*)

}
