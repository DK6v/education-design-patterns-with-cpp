#include "boost/type_index.hpp"
#include <gtest/gtest.h>

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

struct DeleteObject {
    template <typename T>
    void operator()(T* ptr) const {
        delete ptr;
    }
};

struct DeleteArray {
    template <typename T>
    void operator()(T* ptr) const {
        delete[] ptr;
    }
};

struct MoveAllowed {};

struct MoveForbiden {
    MoveForbiden() = default;
    MoveForbiden(MoveForbiden&&) = delete;
    MoveForbiden(const MoveForbiden&) = delete;
    MoveForbiden& operator=(MoveForbiden&&) = delete;
    MoveForbiden& operator=(const MoveForbiden&) = delete;
};

#define UNIT_TESTING

#ifdef UNIT_TESTING
template <typename P>
struct TestPolicy {
    auto get() { return static_cast<P*>(this)->ptr_; }
};
#else
template <typename P>
struct TestPolicy {};
#endif

template <typename T,
          typename DeletionPolicy = DeleteObject,
          typename MovePolicy = MoveForbiden>
class smart_ptr :
    private DeletionPolicy,
    private MovePolicy,
    public TestPolicy<smart_ptr<T, DeletionPolicy, MovePolicy>> {
public:
    template <typename U = T>
    explicit smart_ptr(
        T* ptr = nullptr, const DeletionPolicy&& deletionPolicy = DeletionPolicy()
    ) : DeletionPolicy(std::move(deletionPolicy)),
        MovePolicy(),
        ptr_(ptr)
    {}

    smart_ptr(
        smart_ptr&& other
    ) : DeletionPolicy(std::move(other)),
        MovePolicy(std::move(other)),
        ptr_(other.ptr_)
    {
        std::cout << "-- move constructor" << std::endl;
        other.release();
    }

    smart_ptr& operator=(smart_ptr&& other) {

        std::cout << "-- move operator" << std::endl;
        if (this != &other) {
            DeletionPolicy::operator=(std::move(other));
            MovePolicy::operator=(std::move(other));
            ptr_ = other.ptr_;

            other.release();
        }
        return *this;
    }

    ~smart_ptr() { DeletionPolicy::operator()(ptr_); }

    T& operator*() { return *ptr_; }

    T* operator->() { return ptr_; }
    const T* operator->() const { return ptr_; }

    void release() { ptr_ = nullptr; }

    using type_t = T;
    template <typename U> using rebind_type =
        smart_ptr<U, DeletionPolicy, MovePolicy>;

private:
    friend TestPolicy<smart_ptr>;

    smart_ptr(const smart_ptr&) = delete;
    smart_ptr& operator=(const smart_ptr&) = delete;

    T* ptr_;
};

template <typename T,
          typename DeletePolicy = DeleteObject>
using move_ptr = smart_ptr<T, DeletePolicy, MoveAllowed>;

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(StrategyTemplate, Test01) {
    {
        smart_ptr ptr(new Base);
        (*ptr).value = 1;
        EXPECT_EQ(1, (*ptr).value);
    }
    std::cout << "--" << std::endl;
}

TEST(StrategyTemplate, Test02) {
    {
        auto from = move_ptr(new Base);
        EXPECT_NE(nullptr, from.get());

        (*from).value = 1;
        EXPECT_EQ(1, (*from).value);

        auto ptr(std::move(from));

        EXPECT_EQ(nullptr, from.get());
        EXPECT_NE(nullptr, ptr.get());
        EXPECT_EQ(1, (*ptr).value);

    }
    std::cout << "--" << std::endl;
}

TEST(StrategyTemplate, Test03) {
    {
        move_ptr<Base> ptr;
        ptr = std::move(move_ptr<Base>(new Base()));

        (*ptr).value = 1;
        EXPECT_EQ(1, (*ptr).value);
    }
    std::cout << "--" << std::endl;
}

TEST(StrategyTemplate, Test04) {
    {
        move_ptr ptr(new Base);
        EXPECT_NE(nullptr, ptr.get());

        (*ptr).value = 1;
        EXPECT_EQ(1, (*ptr).value);

        // Create a copy using the type specified by the original pointer.
        move_ptr<decltype(ptr)::type_t> copy(std::move(ptr));

        // Copy all policies from the original pointer, but change the type.
        auto i_ptr = decltype(ptr)::rebind_type<int>(new int);
    }
    std::cout << "--" << std::endl;
}
