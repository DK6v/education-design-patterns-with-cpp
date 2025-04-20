#include "boost/type_index.hpp"
#include "common.h"

template <typename T>
void foo_lv(T& v) {
    std::cout << "   "
              << "T: " << boost::typeindex::type_id_with_cvr<T>().pretty_name() << " "
              << (std::is_lvalue_reference<T>::value ? "lvalue"
                    : (std::is_rvalue_reference<T>::value  ? "rvalue"

                        : "none")) << ", "
              << "v: " << boost::typeindex::type_id_with_cvr<decltype(v)>().pretty_name() << " "
              << (std::is_lvalue_reference<decltype(v)>::value ? "lvalue"
                    : (std::is_rvalue_reference<decltype(v)>::value  ? "rvalue"

                        : "none"))
              << std::endl;
}

template <typename T>
void foo_clv(const T& v) {
    std::cout << "   "
              << "T: " << boost::typeindex::type_id_with_cvr<T>().pretty_name() << " "
              << (std::is_lvalue_reference<T>::value ? "lvalue"
                    : (std::is_rvalue_reference<T>::value  ? "rvalue"

                        : "none")) << ", "
              << "v: " << boost::typeindex::type_id_with_cvr<decltype(v)>().pretty_name() << " "
              << (std::is_lvalue_reference<decltype(v)>::value ? "lvalue"
                    : (std::is_rvalue_reference<decltype(v)>::value  ? "rvalue"

                        : "none"))
              << std::endl;
}

template <typename T>
void foo_rv(T&& v) {
    std::cout << "   "
              << "T: " << boost::typeindex::type_id_with_cvr<T>().pretty_name() << " "
              << (std::is_lvalue_reference<T>::value ? "lvalue"
                    : (std::is_rvalue_reference<T>::value  ? "rvalue"

                        : "none")) << ", "
              << "v: " << boost::typeindex::type_id_with_cvr<decltype(v)>().pretty_name() << " "
              << (std::is_lvalue_reference<decltype(v)>::value ? "lvalue"
                    : (std::is_rvalue_reference<decltype(v)>::value  ? "rvalue"

                        : "none"))
              << std::endl;
}

int main(int argc, char** argv) {

    (void)argc;
    (void)argv;

    int a = 1; // lvalue

    std::cout << "== foo_lv(T&) + lvalue " << std::endl;
    foo_lv(a);

    std::cout << "== foo_clv(const T&) + rvalue " << std::endl;
    foo_clv(1);

    std::cout << "== foo_clv(const T&) + lvalue " << std::endl;
    foo_clv(a);

    std::cout << std::endl;

    std::cout << "== foo_rv(T&&) + lvalue " << std::endl;
    foo_rv(a);

    std::cout << "== foo_rv(T&&) + rvalue " << std::endl;
    foo_rv(1);

    std::cout << "== foo_rv(T&&) + xvalue (move) " << std::endl;
    foo_rv(std::move(a));

    return 0;
}