#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

template <typename D>

struct has_operator_less_and_equal {
    friend bool operator!=(const D& lhs, const D& rhs) {

        return !(lhs == rhs);
    }
    friend bool operator>(const D& lhs, const D& rhs) {

        return !(lhs < rhs) && !(lhs == rhs);
    }
};

class A : public has_operator_less_and_equal<A> {
public:
    A(int n) : n_(n) {}

    bool operator<(const A& rhs) const { return rhs.n_ < n_; }
    bool operator==(const A& rhs) const { return rhs.n_ == n_; }

protected:
    int n_;
};

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(CRTP_Delegate, Test1) {
    A a1 = 1;
    A a2 = 2;

    // from defived class
    EXPECT_TRUE(a1 < a2);
    EXPECT_FALSE(a2 < a1);
    EXPECT_TRUE(a1 == a1);
    EXPECT_FALSE(a1 == a2);

    // from base class
    EXPECT_TRUE(a2 > a1);
    EXPECT_FALSE(a1 > a2);
    EXPECT_TRUE(a1 != a2);
    EXPECT_FALSE(a1 != a1);

    // Operators are free functions, so arguments can be casted to correct type.
    EXPECT_TRUE(a2 > 1);
    EXPECT_FALSE(a2 > 2);
    EXPECT_TRUE(2 > a1);
    EXPECT_FALSE(1 > a1);
}