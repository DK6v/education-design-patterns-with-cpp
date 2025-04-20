#include "boost/type_index.hpp"
#include <gtest/gtest.h>

#include <string>
#include <mutex>

class has_unique_id {
public:
    has_unique_id() : id_(++total_) {}
    virtual int id() const { return id_; }
private:
    mutable int id_; // cannot be const, otherwise cannot generate default constructors
    static inline int total_ = 0;
};

template <typename T>
struct printable {
    friend std::ostream& operator<<(std::ostream& out, const T& rhs) {
        out << boost::typeindex::type_id_with_cvr<decltype(rhs)>().pretty_name()
            << "[" << rhs.id() << "]";
        return out;

    }
};

class Base : public has_unique_id, public printable<Base> {
public:
    Base(int value = 0) : value_(value) { std::cout << "-- " << *this << ": constructor" << std::endl; };
    virtual ~Base() { std::cout << "-- " << *this << ": destructor" << std::endl; };

    virtual void whoami() const { std::cout << *this << " value: " << value_ << std::endl; }

    void operator()(std::string arg) const {
        std::cout << *this << " call operator(): " << arg << std::endl;
    }

public:
    int value_;
};

void foo(std::string arg) {
    std::cout << "call foo(): " << arg << std::endl;
}

template<typename Callable>
class DebugDecorator {
public:
    DebugDecorator(const Callable& cb) : cb_(cb) {}

    template <typename ... Args>
    auto operator()(Args&& ... args) const {
        std::cout << " + DebugDecorator" << std::endl;
        return cb_(std::forward<Args>(args) ...);
    }

private:
    const Callable& cb_;
};

template<typename Callable>
class LockDecorator {
public:
    LockDecorator(const Callable& cb, std::mutex& mutex)
        : cb_(cb), mutex_(mutex) {}

    template <typename ... Args>
    auto operator()(Args&& ... args) const {
        std::cout << " + LockDecorator" << std::endl;
        std::lock_guard<std::mutex> lock(mutex_);

        return cb_(std::forward<Args>(args) ...);
    }

private:
    const Callable& cb_;
    std::mutex& mutex_;
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(Decorator, Test01) {
    {
        std::unique_ptr<Base> base_p(new Base(1));
        base_p->whoami();
        (*base_p)("A");
    }
    std::cout << "-- end of context" << std::endl;
}

TEST(Decorator, Test02) {
    {
        Base base(2);
        base.whoami();

        std::cout << "-- Decorate " << std::endl;
        DebugDecorator decorator(base);
        decorator("B");
    }
    std::cout << "-- end of context" << std::endl;
}

TEST(Decorator, Test03) {
    {
        foo("C");

        std::cout << "-- Decorate " << std::endl;
        DebugDecorator decorator(foo);
        decorator("C");
    }
    std::cout << "-- end of context" << std::endl;
}

TEST(Decorator, Test04) {
    {
        auto bar = [](std::string arg) {
            std::cout << "call foo(): " << arg << std::endl;
        };
        bar("D");

        std::cout << "-- Add debug decorator " << std::endl;
        DebugDecorator first(bar);
        first("D");

        std::cout << "-- Add lock decorator " << std::endl;
        std::mutex lock;
        LockDecorator second(first, lock);
        second("D");
    }
    std::cout << "-- end of context" << std::endl;
}
