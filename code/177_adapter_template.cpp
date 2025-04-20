#include "boost/type_index.hpp"
#include <gtest/gtest.h>

#include <vector>
#include <functional>

using namespace std::placeholders;

template <template <typename> class Contailer, typename T>
std::ostream& operator<<(std::ostream& out, const Contailer<T>& rhs) {
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

// NOTE!
// 1. Wrapper for vector with one template parameter, it can be printed
//    by operator<<(...) above.
// 2. You cannot use aliases when inferring the type of a template argument.
template <typename T>
struct vector_t : public std::vector<T> {
    using std::vector<T>::vector;
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(Adapter, Test01) {
    {
        auto vect = vector_t<int>({ 1, 2, 3, 4, 5 });
        std::cout << "Vector: " << vect << std::endl;
        EXPECT_EQ(vect, std::vector({ 1, 2, 3, 4, 5 }));
    }
    std::cout << "-- end of context" << std::endl;
}
