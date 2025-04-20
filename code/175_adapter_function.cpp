#include "boost/type_index.hpp"
#include <gtest/gtest.h>

#include <vector>

struct reverse_sort_less {
    template <typename T>
    bool operator()(const T& lhs, const T& rhs) {
        return lhs > rhs;
    }
};

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& rhs) {
    out << "{";
    auto it = rhs.cbegin();
    while (it != rhs.cend()) {
        out << *it;
        if (++it != rhs.cend()) {
            out << ", ";
        }
    }
    out << "}";
    return out;

}

template <typename Iter>
void reverse_sort(Iter begin, Iter end) {
    std::sort(begin, end, reverse_sort_less());
}

// NOTE!
// You cannot use aliases when inferring the type of a template argument.
template <typename Container>
void reverse_sort(Container& container) {
    std::sort(container.begin(), container.end(), reverse_sort_less());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(Adapter, Test01) {
    {
        std::vector<int> vect = { 1, 2, 3 };
        std::cout << "Vector: " << vect << std::endl;

        std::cout << "-- reverse sort" << std::endl;
        std::sort(vect.begin(), vect.end(), reverse_sort_less());
        std::cout << "Vector: " << vect << std::endl;

        EXPECT_EQ(vect, std::vector({ 3, 2, 1 }));
    }
    std::cout << "-- end of context" << std::endl;
}

TEST(Adapter, Test02) {
    {
        std::vector<int> vect = { 1, 2, 3 };
        std::cout << "Vector: " << vect << std::endl;

        std::cout << "-- reverse sort" << std::endl;
        reverse_sort(vect.begin(), vect.end());
        std::cout << "Vector: " << vect << std::endl;

        EXPECT_EQ(vect, std::vector({ 3, 2, 1 }));
    }
    std::cout << "-- end of context" << std::endl;
}

TEST(Adapter, Test03) {
    {
        std::vector<int> vect = { 1, 2, 3 };
        std::cout << "Vector: " << vect << std::endl;

        std::cout << "-- reverse sort" << std::endl;
        reverse_sort(vect);
        std::cout << "Vector: " << vect << std::endl;

        EXPECT_EQ(vect, std::vector({ 3, 2, 1 }));
    }
    std::cout << "-- end of context" << std::endl;
}

TEST(Adapter, Test04) {
    {
        std::vector<int> vect = { 1, 2, 3 };
        std::cout << "Vector: " << vect << std::endl;

        std::cout << "-- reverse sort" << std::endl;
        auto less = [](const auto& lhs, const auto& rhs) {
            return rhs < lhs;
        };
        std::sort(vect.begin(), vect.end(), less);
        std::cout << "Vector: " << vect << std::endl;

        EXPECT_EQ(vect, std::vector({ 3, 2, 1 }));
    }
    std::cout << "-- end of context" << std::endl;
}
