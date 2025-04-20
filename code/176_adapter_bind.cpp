#include "boost/type_index.hpp"
#include <gtest/gtest.h>

#include <vector>
#include <functional>

using namespace std::placeholders;

template <typename T, typename... Args>
auto make_vector(Args... args) {
    return std::vector<T>({ args... });
}

auto func5(int a, int b, int c, int d, int e) {
    return std::vector<int>({a, b, c, d, e});
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& rhs) {
    out << "{";
    bool first = true;
    for (const auto& item: rhs) {
        if (!first) {
            out << ", ";
        }
        first = false;
        out << item;
    }
    out << "}";
    return out;

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(Adapter, Test01) {
    {
        auto vect = make_vector<int>(1, 2, 3, 4, 5);
        std::cout << "Vector: " << vect << std::endl;
        EXPECT_EQ(vect, std::vector({ 1, 2, 3, 4, 5 }));
    }
    std::cout << "-- end of context" << std::endl;
}

TEST(Adapter, Test02) {
    {
        auto makeV5 = std::bind(func5, _1, _2, _3, 0, 0);
        auto vect = makeV5(1, 2, 3);

        std::cout << "Vector: " << vect << std::endl;
        EXPECT_EQ(vect, std::vector({ 1, 2, 3, 0, 0}));
    }
    std::cout << "-- end of context" << std::endl;
}
