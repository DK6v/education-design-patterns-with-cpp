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

template <typename T>
struct DeleteObjectType {
    void operator()(T* ptr) const {
        delete ptr;
    }
};

template <typename T>
struct DeleteArrayType {
    void operator()(T* ptr) const {
        delete[] ptr;
    }
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

template <typename P>
class WithRelease {
public:
    void release() {
        static_cast<P*>(this)->ptr_ = nullptr;
    }
};

template <typename P>
class NoRelease {};

struct WithAsteriskOperator {
    static constexpr bool have_asterisk_operator = true;
};

struct WithoutAsteriskOperator {
    static constexpr bool have_asterisk_operator = false;
};

struct MoveAllowed {
    static constexpr bool can_move = true;
};

struct MoveForbiden {
    static constexpr bool can_move = false;
};

template <typename T,
          typename DeletionPolicy = DeleteObject,
          template <typename> typename ReleasePolicy = NoRelease,
          typename MovePolicy = MoveForbiden,
          typename AsteriskPolicy = WithAsteriskOperator>
class smart_ptr :
    public DeletionPolicy,
    public ReleasePolicy<smart_ptr<T, DeletionPolicy, ReleasePolicy>> {
public:
    explicit smart_ptr(T* ptr, const DeletionPolicy&& deletionPolicy = DeletionPolicy()
    ) : DeletionPolicy(std::move(deletionPolicy)), ptr_(ptr) {}

    template <typename U,
              typename V = std::enable_if_t<
                MovePolicy::can_move && std::is_same<U, smart_ptr>::value, U>>
    smart_ptr(U&& other) :
        DeletionPolicy(std::move(other)),
        ptr_(other.ptr_)
    {
        other.release();
    }

    ~smart_ptr() { DeletionPolicy::operator()(ptr_); }

    template <typename U = T>
    std::enable_if_t<AsteriskPolicy::have_asterisk_operator, U&>
    operator*() { return *ptr_; }

    T* operator->() { return ptr_; }
    const T* operator->() const { return ptr_; }

    const T* get() const { return ptr_; }

private:
    // smart_ptr(const smart_ptr&) = delete;
    smart_ptr& operator=(const smart_ptr&) = delete;

    friend class ReleasePolicy<smart_ptr>;
    T* ptr_;
};

template <typename T,
          typename AsteriskPolicy = WithAsteriskOperator>
using object_ptr_t = smart_ptr<T, DeleteObject, WithRelease, MoveAllowed, AsteriskPolicy>;

template <typename T,
          typename MovePolicy = MoveAllowed>
using move_ptr_t = smart_ptr<T, DeleteObject, WithRelease, MovePolicy, WithAsteriskOperator>;

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(StrategyTemplate, Test01) {
    {
        object_ptr_t ptr(new Base);
        (*ptr).value = 1;
        EXPECT_EQ(1, (*ptr).value);
    }
    std::cout << "--" << std::endl;
}

TEST(StrategyTemplate, Test02) {
    {
        object_ptr_t<Base, WithoutAsteriskOperator> ptr(new Base);

        // compilation error, asterisk operator is not allowed
        // (*ptr).value

        ptr->value = 1;
        EXPECT_EQ(1, ptr->value);
    }
    std::cout << "--" << std::endl;
}

TEST(StrategyTemplate, Test03) {
    {
        move_ptr_t<Base, MoveAllowed> a(new Base);

        a->value = 1;
        EXPECT_EQ(1, a->value);
        EXPECT_NE(nullptr, a.get());

        move_ptr_t b(std::move(a));
        EXPECT_EQ(nullptr, a.get());

        EXPECT_EQ(1, b->value);
        EXPECT_NE(nullptr, b.get());
    }
    std::cout << "--" << std::endl;
}

TEST(StrategyTemplate, Test04) {
    {
        move_ptr_t<Base, MoveForbiden> a(new Base);

        a->value = 1;
        EXPECT_EQ(1, a->value);
        EXPECT_NE(nullptr, a.get());

        // Move forbiden, compilation error
        // strict_ptr_t<Base, MoveForbiden> b(std::move(a));
    }
    std::cout << "--" << std::endl;
}
