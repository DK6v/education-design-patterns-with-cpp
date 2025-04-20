#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include "common.h"

class Object {
public:
    virtual bool less(const Object* rhs) const = 0;

    virtual bool operator<(const Object& rhs) const {
        return less(&rhs);
    };
    virtual bool operator==(const Object& rhs) const {
        return !less(&rhs) && !rhs.less(this);
    };
};

class Int : public Object {
public:
    Int(int value) : value(value) {}
    virtual bool less(const Object* rhs) const override {
        // dynamic_cast safely converts pointers and references to classes
        return (value < dynamic_cast<const Int*>(rhs)->value);
    }

public:
    int value;
};

bool less(const Object& lhs, const Object& rhs) {
    return lhs.less(&rhs);
}

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(Test, CreateObjects) {
    Int a(1), b(2);
    EXPECT_TRUE(a.less(&b));
    EXPECT_TRUE(less(a, b));

    Object* p = &a;
    Object* q = &b;
    EXPECT_TRUE(p->less(q));
    EXPECT_TRUE(less(*p, *q));

    EXPECT_TRUE(a == Int(1));
}

TEST(Test, SortObjects) {

    // Type erase is not supported for arrays of Objects,
    // as the size of each object is unknown.
    // An abstract iterator needs to be implemented.

    Int a[3] = {3, 1, 2};

    auto greaterThan = [](const Int& lhs, const Int& rhs) -> bool {
        return !less(lhs, rhs) && !(lhs == rhs);
    };

    std::sort(a, a + 3, greaterThan);
    EXPECT_EQ(Int(3), a[0]);
    EXPECT_EQ(Int(2), a[1]);
    EXPECT_EQ(Int(1), a[2]);

    std::sort(std::begin(a), std::end(a));
    EXPECT_EQ(Int(1), a[0]);
    EXPECT_EQ(Int(2), a[1]);
    EXPECT_EQ(Int(3), a[2]);
}
