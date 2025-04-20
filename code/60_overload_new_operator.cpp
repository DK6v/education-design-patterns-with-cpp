#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include "common.h"

namespace app {

class Heap {
public:
    void* allocate(size_t sz) {
        ++count;
        return std::malloc(sz);
    }
    void free(void* ptr) {
        --count;
        return std::free(ptr);
    }

    class Deleter {
    public:
        Deleter(Heap* heap) : heap_(heap) {}
        template <typename T>
        void operator()(T* ptr) {
            ptr->~T();          // <- call destructor manually
            heap_->free(ptr);   // <- free memory
        }
    private:
        Heap* heap_;
    };

public:
    static inline int count = 0;
};

} // namespace app

// Allocation fuctions can only be used within object or global scope.
// The delete operator cannot be specified, as it has no arguments.
void* operator new(size_t sz, app::Heap heap) noexcept {
    return heap.allocate(sz);
}

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(Test, HeapAllocateFree) {
    app::Heap heap;
    EXPECT_EQ(0, heap.count);

    int* p = new(heap) int(123);
    EXPECT_NE(nullptr, p);
    EXPECT_EQ(123, *p);
    EXPECT_EQ(1, heap.count);

    heap.free(p);
    EXPECT_EQ(0, heap.count);
}

TEST(Test, HeapAllocateDeleter) {
    app::Heap heap;
    app::Heap::Deleter deleter(&heap);

    EXPECT_EQ(0, heap.count);

    {
        std::unique_ptr<int, app::Heap::Deleter> p(
            new(heap) int(456),
            app::Heap::Deleter(&heap)
        );

        EXPECT_NE(nullptr, p);
        EXPECT_EQ(456, *p);
        EXPECT_EQ(1, heap.count);
    }

    EXPECT_EQ(0, heap.count);
}

TEST(Test, SharedPtrWithDeleter) {
    app::Heap heap;
    app::Heap::Deleter deleter(&heap);

    EXPECT_EQ(0, heap.count);

    {
        std::shared_ptr<int> p(
            new(heap) int(456),
            app::Heap::Deleter(&heap)
        );

        EXPECT_NE(nullptr, p);
        EXPECT_EQ(456, *p);
        EXPECT_EQ(1, heap.count);
    }

    EXPECT_EQ(0, heap.count);
}

TEST(Test, TypeErase) {
    app::Heap heap;
    app::Heap::Deleter deleter(&heap);

    std::shared_ptr<int> p(
        new(heap) int(456),
        app::Heap::Deleter(&heap)
    );

    EXPECT_EQ(1, p.use_count());

    std::shared_ptr<int> q(p);
    EXPECT_EQ(2, q.use_count());

    // Pass by reference
    auto foo = [](std::shared_ptr<int>& d) {
        EXPECT_EQ(2, d.use_count());
    };
    foo(q);
    EXPECT_EQ(2, q.use_count());

    // Pass by value
    auto bar = [](std::shared_ptr<int> d) {
        EXPECT_EQ(3, d.use_count());
    };
    bar(q);
    EXPECT_EQ(2, q.use_count());
}