#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include "common.h"

struct ObjectCounter {
    static inline int count = 0;
    static inline int total = 0;
    ObjectCounter() { ++count; ++total; }
    ~ObjectCounter() { --count; }
};

template <typename T>
class ScopedPtr : public ObjectCounter {
public:
    ScopedPtr(T* ptr) : ptr(ptr) {}
    ~ScopedPtr() { delete ptr; }
    T* get() { return ptr; }
    T& operator*() { return *ptr; }
    void release() {
        delete ptr;
        ptr = nullptr; // is's allowed to delete nullptr then;
    }

private:
    ScopedPtr(const ScopedPtr&) = delete;
    ScopedPtr& operator=(const ScopedPtr&) = delete;

    T* ptr;
};

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(Test, ReleaseScopedPtr) {

    ScopedPtr p1(new int);
    EXPECT_NE(p1.get(), nullptr);

    EXPECT_EQ(1, ScopedPtr<int>::count);
    EXPECT_EQ(1, ScopedPtr<int>::total);

    try {
        ScopedPtr p2 = new int;
        EXPECT_NE(p2.get(), nullptr);

        EXPECT_EQ(2, ScopedPtr<int>::count);
        EXPECT_EQ(2, ScopedPtr<int>::total);

        p2.release();
        EXPECT_EQ(p2.get(), nullptr);

        throw std::runtime_error(::testing::UnitTest::GetInstance()->current_test_info()->name());
    }
    catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    EXPECT_EQ(1, ScopedPtr<int>::count);
    EXPECT_EQ(2, ScopedPtr<int>::total);
}

