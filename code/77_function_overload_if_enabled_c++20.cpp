#include "boost/type_index.hpp"
#include <gtest/gtest.h>

#include <vector>
#include <list>

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

template <typename T, bool if_have_sort, bool if_have_range>
struct fast_sort_helper;

template <typename T, bool if_have_range>
struct fast_sort_helper<T, true, if_have_range> {
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
        if_have_sort<T>::value,
        if_have_range<T>::value>::fast_sort(c);
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
