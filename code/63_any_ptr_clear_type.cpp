#include "boost/type_index.hpp"

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

template <typename T>
class has_unique_id {
public:
    has_unique_id() : id_(++total_) {}
    virtual int id() const { return id_; }
private:
    mutable int id_; // cannot be const, otherwise cannot generate default constructors
    static inline int total_ = 0;
};

template <typename T>
struct printable : public has_unique_id<T> {
    friend std::ostream& operator<<(std::ostream& out, const T& rhs) {
        out << boost::typeindex::type_id_with_cvr<decltype(rhs)>().pretty_name()
            << "[" << rhs.id() << "]";
        return out;

    }
};

class Base : public printable<Base> {
public:
    Base() : value(0) { std::cout << "-- " << *this << ": constructor" << std::endl; };
    virtual ~Base() { std::cout << "-- " << *this << ": destructor" << std::endl; };
public:
    int value;
};

struct ObjectDeleter {
    template<typename T>
    void operator() (T* ptr) { delete ptr; }
};

class any_type {
public:
    template <typename T,
              typename DeletePolicy = ObjectDeleter>
    any_type(
        const T& value,
        const DeletePolicy&& deleter = DeletePolicy()
    ) :

        storage_(new storage_t<T>(value))
    {
        (void)deleter;
    }

    ~any_type() {
        delete storage_;
    }

    template <typename T>
    T& operator*() { return *(static_cast<T*>(storage_->get())); }

private:

    any_type(any_type&&) = delete;
    any_type operator=(const any_type&) = delete;

    struct storage_base_t {
        virtual void* get() = 0;
        virtual ~storage_base_t() = default;
    };

    template <typename T>
    struct storage_t : public storage_base_t {
        storage_t(const T& value) : value_(value) {}

        void* get() override { return &value_; }
        T value_;
    };

private:
    storage_base_t* storage_;
};

template <typename T> T& any_type_cast(any_type& variable) {
    return variable.operator*<T>();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(StrategyTemplate, Test01) {
    {
        any_type a(1);

        a.operator*<int>() = 1;
        EXPECT_EQ(1, a.operator*<int>());

        any_type_cast<int>(a) = 2;
        EXPECT_EQ(2, any_type_cast<int>(a));
    }
    std::cout << "-- END" << std::endl;
}