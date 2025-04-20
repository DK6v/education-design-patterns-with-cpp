#include <mutex>

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include "common.h"

using UnitTest = ::testing::UnitTest;

class MutexGuard {
public:
    explicit MutexGuard(std::mutex& m) : m_(m) { m_.lock(); }
    ~MutexGuard() { reset(); }

    void reset() {
        if (mustUnlock_) {
            m_.unlock();
            mustUnlock_ = false;
        }
    }
private:
    MutexGuard(const MutexGuard&) = delete;
    MutexGuard& operator=(const MutexGuard&) = delete;

    // Since there is a reference variable, the operator= will not be generated.
    std::mutex& m_;
    bool mustUnlock_ = true;
};

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

std::mutex m;

TEST(Test, MutexLockUnlock) {
    try {
        MutexGuard guard(m);
        EXPECT_FALSE(m.try_lock());
        throw std::runtime_error(::UnitTest::GetInstance()->current_test_info()->name());
    }
    catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    EXPECT_TRUE(m.try_lock());
    m.unlock();
}

TEST(Test, MutexLockReset) {
    try {
        MutexGuard guard(m);
        EXPECT_FALSE(m.try_lock());

        guard.reset();

        EXPECT_TRUE(m.try_lock());

        m.unlock();

        throw std::runtime_error(::UnitTest::GetInstance()->current_test_info()->name());
    }
    catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    EXPECT_TRUE(m.try_lock());
    m.unlock();
}

TEST(Test, LockGuardMutex) {
    try {
        std::lock_guard guard(m);
        EXPECT_FALSE(m.try_lock());
        throw std::runtime_error(::UnitTest::GetInstance()->current_test_info()->name());
    }
    catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    EXPECT_TRUE(m.try_lock());
    m.unlock();
}