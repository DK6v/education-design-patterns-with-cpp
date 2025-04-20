#include "boost/type_index.hpp"
#include <gtest/gtest.h>

#include <vector>
#include <list>
#include <set>

template <typename T>
struct if_have_sort : std::bool_constant<
    requires () {
        static_cast<void(T::*)()>(&T::sort);
    }> {};

template <typename T>
struct if_have_range : std::bool_constant<
    requires () {
         static_cast<typename T::iterator (T::*)()>(&T::begin);
         static_cast<typename T::iterator (T::*)()>(&T::end);
    }> {};

template <typename T>
typename std::enable_if<
    if_have_sort<T>::value, void>::type
fast_sort(T& c) {
    std::cout << "T::sort()" << std::endl;
    c.sort();
}

template <typename T>
typename std::enable_if<(
    !if_have_sort<T>::value && if_have_range<T>::value), void>::type
fast_sort(T& c) {
    std::cout << "std::sort()" << std::endl;
    std::sort(c.begin(), c.end());
}

template <typename T>
typename std::enable_if<(
    !if_have_sort<T>::value && !if_have_range<T>::value), void>::type
fast_sort(T& c) {
    static_assert(false, "Sort method is unknown.");
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

    std::set<int> myset = {3, 1, 2};
    // compilation failed, assert
    // fast_sort(myset);
}
