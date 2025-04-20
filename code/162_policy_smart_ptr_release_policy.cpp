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
    void release() { static_cast<P*>(this)->ptr_ = nullptr; }
};

template <typename P>
class NoRelease {};

template <typename T,
          typename DeletionPolicy = DeleteObject,
          template <typename> typename ReleasePolicy = NoRelease>
class smart_ptr :
    public DeletionPolicy,
    public ReleasePolicy<smart_ptr<T, DeletionPolicy, ReleasePolicy>> {
public:
    explicit smart_ptr(T* ptr, const DeletionPolicy&& deletionPolicy = DeletionPolicy()
    ) : DeletionPolicy(std::move(deletionPolicy)), ptr_(ptr) {}

    ~smart_ptr() { DeletionPolicy::operator()(ptr_); }

    T& operator*() { return *ptr_; }
    T* operator->() { return ptr_; }
    const T* operator->() const { return ptr_; }

private:
    smart_ptr(const smart_ptr&) = delete;
    smart_ptr& operator=(const smart_ptr&) = delete;

    friend class ReleasePolicy<smart_ptr>;
    T* ptr_;
};

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
        smart_ptr ptr(new Base);

        ptr->value = 1;
        EXPECT_EQ(1, ptr->value);

        ptr.operator->()->value = 2;
        EXPECT_EQ(2, (*ptr).value);

        (*ptr.operator->()).value = 3;
        EXPECT_EQ(3, (*ptr).value);
    }
    std::cout << "--" << std::endl;
}

TEST(StrategyTemplate, Test03) {
    {
        Base* a = new Base;
        smart_ptr<const Base> ptr(a);

        a->value = 1;
        EXPECT_EQ(1, ptr->value);

        a->value = 2;
        EXPECT_EQ(2, (*ptr).value);

        a->value = 3;
        EXPECT_EQ(3, (*ptr).value);
    }
    std::cout << "--" << std::endl;
}

TEST(StrategyTemplate, Test04) {
    {
        smart_ptr<Base, DeleteArray> ptr(new Base[2]);
    }
    std::cout << "--" << std::endl;
}

TEST(StrategyTemplate, Test05) {
    DeleteArrayType<int> a;
    DeleteArrayType<long> b;

    EXPECT_TRUE(sizeof(a) != 0);
    EXPECT_TRUE(sizeof(b) != 0);
    EXPECT_TRUE(static_cast<void*>(&a) != static_cast<void*>(&b));

    std::cout << "-- DeleteArrayType<int>&  : " << &a << " sz: " << sizeof(a) << std::endl;
    std::cout << "-- DeleteArrayType<long>& : " << &b << " sz: " << sizeof(a) << std::endl;

    std::cout << "--" << std::endl;
}

TEST(StrategyTemplate, Test06) {
    Base* pa = new Base;
    {
        smart_ptr<Base, DeleteObject, WithRelease> ptr(pa);
        ptr.release();

        std::cout << "--" << std::endl;
    }
    delete pa;
}
