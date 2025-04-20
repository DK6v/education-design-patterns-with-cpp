#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

template <typename D>

struct with_quantity_counter {
protected:
    static int quantity_;
    with_quantity_counter() { ++quantity_; }
    with_quantity_counter(const with_quantity_counter&) { ++quantity_; }

public:
    ~with_quantity_counter() { --quantity_; }

    static int quantity_counter() {
        return quantity_;
    }
};

template <typename D> int with_quantity_counter<D>::quantity_ = 0;

class A : public with_quantity_counter<A> {
public:
    A() = default;
};

class B : public with_quantity_counter<B> {
public:
    B() = default;
};

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(CRTP_ClassCounter, Test1) {
    A a1;
    EXPECT_EQ(1, A::quantity_counter());

    B b1;
    B b2;
    EXPECT_EQ(1, A::quantity_counter());
    EXPECT_EQ(2, B::quantity_counter());

    {
        B b3;
        EXPECT_EQ(1, A::quantity_counter());
        EXPECT_EQ(3, B::quantity_counter());
    }

    EXPECT_EQ(1, A::quantity_counter());
    EXPECT_EQ(2, B::quantity_counter());

    B b4(b2); // copy constructor
    B b5 = b4;

    EXPECT_EQ(1, A::quantity_counter());
    EXPECT_EQ(4, B::quantity_counter());
}