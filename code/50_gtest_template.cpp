#include <gtest/gtest.h>

struct ObjectCounter {

    static inline int count = 0;
    static inline int total = 0;

    ObjectCounter() {
        ++count;
        ++total;
    }

    ~ObjectCounter() {
        --count;
    }
};

// int object_counter::count = 0;

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(ObjectCounter, AcquireRelease) {

    ObjectCounter::count = 0;
    ObjectCounter::total = 0;

    ObjectCounter* ptr = new ObjectCounter;
    EXPECT_EQ(1, ObjectCounter::count);
    EXPECT_EQ(1, ObjectCounter::total);

    delete ptr;
    EXPECT_EQ(0, ObjectCounter::count);
    EXPECT_EQ(1, ObjectCounter::total);
}
