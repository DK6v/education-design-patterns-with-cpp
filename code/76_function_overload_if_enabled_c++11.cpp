#include "boost/type_index.hpp"
#include <gtest/gtest.h>

#include <vector>
#include <list>

enum test_result { YES = 1, NO = 2 };
struct yes { int _[YES]; };
struct no  { int _[NO]; };
static_assert(sizeof(yes) != sizeof(no), "Structs must have different sizes.");

template <typename T> yes
    test_have_sort(decltype(static_cast<void(T::*)()>(&T::sort)));
template <typename T> no
    test_have_sort(...);

template <typename T> yes
    test_have_range(decltype(static_cast<typename T::iterator (T::*)()>(&T::begin)),
                    decltype(static_cast<typename T::iterator (T::*)()>(&T::end)));
template <typename T> no
    test_have_range(...);

template <typename T, bool have_sort, bool have_range>
struct fast_sort_helper;

template <typename T>
struct fast_sort_helper<T, true, true> {
    static void fast_sort(T& c) {
        std::cout << "T::sort()" << std::endl;
        c.sort();
    }
};

template <typename T>
struct fast_sort_helper<T, true, false> {
    static void fast_sort(T& c) {
        std::cout << "T::sort()" << std::endl;
        c.sort();
    }
};

template <typename T>
struct fast_sort_helper<T, false, true> {
    static void fast_sort(T& c) {
        std::cout << "std::sort()" << std::endl;
        std::sort(c.begin(), c.end());
    }
};

template <typename T>
struct fast_sort_helper<T, false, false> {
    static void fast_sort(T& c) {
        static_assert(false, "Sort method is unknown.");
    }
};

template <typename T>
void fast_sort(T& c) {
    fast_sort_helper<T,
        sizeof(test_have_sort<T>(NULL)) == sizeof(yes),
        sizeof(test_have_range<T>(NULL, NULL)) == sizeof(yes)>::fast_sort(c);
}

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(FunctionOverloadIfEnabled, Test1) {
    std::vector<int> vect = {3, 1, 2};
    fast_sort(vect);
    EXPECT_EQ(vect, std::vector({1, 2, 3}));

    std::list<int> list = {3, 1, 2};
    fast_sort(list);
    EXPECT_EQ(list, std::list({1, 2, 3}));
}
