#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include "common.h"

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(Test, ScopedUniquePtr) {

    std::unique_ptr<int> p1(new int);
    EXPECT_NE(nullptr, p1.get());

    {
        std::unique_ptr<int> p2(new int);
        int* raw_p = p2.get();

        p1 = std::move(p2);
        EXPECT_NE(nullptr, p1.get());

        EXPECT_EQ(raw_p, p1.get());
        EXPECT_EQ(nullptr, p2.get());

        const std::unique_ptr<int> p3(new int);
        // Compilation error
        // p1 = std::move(p3);
        // p3 = std::move(p1);
    }
}

