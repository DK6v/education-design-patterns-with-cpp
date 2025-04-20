#include "boost/type_index.hpp"
#include <gtest/gtest.h>

#include <queue>
#include <mutex>
#include <string>

template <typename T>
class locking_queue {
public:
    using value_type = typename std::queue<T>::value_type;

    void push(const value_type& value) {
        lock_guard lock(mutex_);
        queue_.push(value);
    }

    void push(value_type&& value) {
        lock_guard lock(mutex_);
        queue_.push(value);
    }

    bool pop(value_type& out) {
        lock_guard lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        out = queue_.front();
        queue_.pop();
        return true;
    }

    std::pair<value_type, bool> pop() {
        lock_guard lock(mutex_);
        if (queue_.empty()) {
            return { value_type(), false };
        }
        value_type out = std::move(queue_.front());
        queue_.pop();
        return { out, true };
    }

    friend std::ostream& operator<<(std::ostream& out, const locking_queue<T>& rhs) {
        out << "Queue: {";
        auto temp = rhs.queue_;
        while(!temp.empty()) {
            out << temp.front();
            temp.pop();
            if (temp.size() != 0) {
                out << ", ";
            }
        }
        out << "}";
        return out;

    }

private:
    using mutex = std::mutex;
    using lock_guard = std::lock_guard<mutex>;

    std::queue<T> queue_;
    mutex mutex_;

};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(Adapter, Test01) {
    {
        locking_queue<std::string> queue;

        std::cout << "-- push" << std::endl;
        queue.push("A");
        queue.push("B");
        queue.push("C");
        std::cout << queue << std::endl;

        std::cout << "-- pop" << std::endl;
        std::string out;
        EXPECT_TRUE(queue.pop(out));
        EXPECT_EQ(out, "A");
        std::cout << queue << std::endl;
    }
    std::cout << "-- end of context" << std::endl;
}

TEST(Adapter, Test02) {
    {
        locking_queue<std::string> queue;

        std::cout << "-- push" << std::endl;
        queue.push("A");
        queue.push("B");
        queue.push("C");
        std::cout << queue << std::endl;

        std::cout << "-- pop" << std::endl;
        auto out = queue.pop();
        EXPECT_TRUE(out.second);
        EXPECT_EQ(out.first, "A");
        std::cout << queue << std::endl;
    }
    std::cout << "-- end of context" << std::endl;
}
